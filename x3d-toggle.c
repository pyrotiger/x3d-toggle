#define _XOPEN_SOURCE 500
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/wait.h>

#define VERSION "1.2.0"
#define AMD_X3D_MODE_GLOB "/sys/devices/platform/AMDI*/amd_x3d_mode"

static void audit_log(int priority, const char *fmt, ...);
static int check_root(void);

static int read_config_value(const char *path, const char *key, char *out, size_t outlen) {
  if (!path || !key || !out) return 1;
  FILE *f = fopen(path, "r");
  if (!f) return 1;
  char line[512];
  int found = 1;
  while (fgets(line, sizeof(line), f)) {
    char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '#' || *p == '\n' || *p == '\0') continue;
    char *eq = strchr(p, '=');
    if (!eq) continue;
    *eq = '\0';
    const char *k = p;
    char *v = eq + 1;
    /* trim newline */
    v[strcspn(v, "\r\n")] = '\0';
    if (strcmp(k, key) == 0) {
      strncpy(out, v, outlen - 1);
      out[outlen - 1] = '\0';
      found = 0;
      break;
    }
  }
  fclose(f);
  return found; /* 0 if found */
}

static const char *get_effective_logmode()
{
  static char buf[64];
  const char *env = getenv("X3D_TOGGLE_LOG");
  if (env && (*env != '\0')) {
    if (strcmp(env, "stdout") == 0 || strcmp(env, "file") == 0 || strcmp(env, "none") == 0) {
      return env;
    }
  }
  /* Fallback to config */
  if (read_config_value("/etc/x3d-toggle.conf", "logmode", buf, sizeof(buf)) == 0) {
    if (strcmp(buf, "stdout") == 0 || strcmp(buf, "file") == 0 || strcmp(buf, "none") == 0) {
      return buf;
    }
  }
  return "none"; /* default: logging off unless enabled by user */
}

static void print_usage(const char *prog_name) {
  printf("x3d-toggle v%s\n", VERSION);
  printf("Usage: %s <command> [args]\n", prog_name);
  printf("Note: For the graphical tool, run 'x3d-toggle-gui' instead.\n\n");
  printf("Commands:\n");
  printf("  cache        Enable 3D V-Cache preference (Rabbit Mode)\n");
  printf("  frequency    Enable Frequency preference (Cheetah Mode)\n");
  printf("  auto         Reset to OS default behavior (Elk Mode)\n");
  printf("  get          Print current mode\n");
  printf("  check-load   Check if CPU usage exceeds threshold (Return 0 if "
         "yes)\n");
  printf("               Arg: <threshold_percent> (default: 50)\n");
  printf("  set          Write configuration: set <key> <value> [config_path]\n");
  printf("  stress       Run a CPU spike test across all cores\n");
  printf("               Arg: <seconds> (1-300, default: 10)\n");
  printf("  log-on       Enable stdout logging (writes logmode=stdout)\n");
  printf("  log-off      Disable logging (writes logmode=none)\n");
  printf("  log-file     Use file logging (writes logmode=file)\n");
  printf("  help         Show this help message\n");
}

static int write_config_atomic(const char *path, const char *key, const char *value) {
  /* Only allow root to write configs */
  if (!check_root()) return 1;

  char tmp_path[512];
  snprintf(tmp_path, sizeof(tmp_path), "%s.tmp.XXXXXX", path);
  int fd = mkstemp(tmp_path);
  if (fd < 0) {
    audit_log(LOG_ERR, "mkstemp failed for %s: %s", tmp_path, strerror(errno));
    return 1;
  }

  FILE *out = fdopen(fd, "w");
  if (!out) {
    audit_log(LOG_ERR, "fdopen failed: %s", strerror(errno));
    close(fd);
    unlink(tmp_path);
    return 1;
  }

  /* Read existing file and copy, replacing key if present */
  FILE *in = fopen(path, "r");
  char line[1024];
  int replaced = 0;
  if (in) {
    while (fgets(line, sizeof(line), in)) {
      const char *eq = strchr(line, '=');
      if (eq) {
        size_t klen = eq - line;
        /* trim newline */
        char keybuf[256];
        strncpy(keybuf, line, klen);
        keybuf[klen] = '\0';
        if (strcmp(keybuf, key) == 0) {
          fprintf(out, "%s=%s\n", key, value);
          replaced = 1;
          continue;
        }
      }
      fputs(line, out);
    }
    fclose(in);
  }

  if (!replaced) {
    fprintf(out, "%s=%s\n", key, value);
  }

  fflush(out);
  fsync(fileno(out));
  fclose(out);

  if (rename(tmp_path, path) != 0) {
    audit_log(LOG_ERR, "rename failed %s -> %s: %s", tmp_path, path, strerror(errno));
    unlink(tmp_path);
    return 1;
  }

  audit_log(LOG_INFO, "Wrote config %s=%s to %s", key, value, path);
  return 0;
}

static int check_root() {
  if (geteuid() != 0) {
    audit_log(LOG_WARNING, "Attempt to run without root privileges (euid=%d)", geteuid());
    fputs("Error: This program must be run as root.\n", stderr);
    return 0;
  }
  return 1;
}

static char *find_x3d_mode_file() {
  glob_t globbuf;
  char *result_path = NULL;

  /* Use GLOB_NOSORT. It directly returns the matched files quickly */
  if (glob(AMD_X3D_MODE_GLOB, GLOB_NOSORT, NULL, &globbuf) == 0) {
    if (globbuf.gl_pathc > 0) {
      result_path = strdup(globbuf.gl_pathv[0]);
    }
  }
  globfree(&globbuf);

  return result_path;
}

static unsigned long long get_cpu_stats(unsigned long long *idle) {
  FILE *fp = fopen("/proc/stat", "r");
  if (idle)
    *idle = 0;
  if (!fp)
    return 0;

  char buffer[1024];
  if (!fgets(buffer, sizeof(buffer), fp)) {
    fclose(fp);
    return 0;
  }
  fclose(fp);

    unsigned long long user, nice, system, idle_ticks, iowait, irq, softirq,
      steal;

  if (strncmp(buffer, "cpu ", 4) != 0) {
    if (idle)
      *idle = 0;
    return 0;
  }

  const char *p = buffer + 4;
  char *endptr = NULL;
  unsigned long long vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < 8; ++i) {
    while (*p == ' ' || *p == '\t')
      ++p;
    if (*p == '\0' || *p == '\n')
      break;
    errno = 0;
    unsigned long long v = strtoull(p, &endptr, 10);
    if (endptr == p || errno != 0)
      break;
    vals[i] = v;
    p = endptr;
  }

  user = vals[0];
  nice = vals[1];
  system = vals[2];
  idle_ticks = vals[3];
  iowait = vals[4];
  irq = vals[5];
  softirq = vals[6];
  steal = vals[7];

  if (idle)
    *idle = idle_ticks + iowait;

  return user + nice + system + idle_ticks + iowait + irq + softirq + steal;
}

static void audit_log(int priority, const char *fmt, ...) {
  char msgbuf[1024];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
  va_end(ap);

  /* Always log to syslog */
  syslog(priority, "%s", msgbuf);

  /* Determine if stdout/stderr should also be used based on logmode and safety */
  const char *mode = get_effective_logmode();
  int allow_stdout = 0;

  /* Explicit environment override to allow stdout even when elevated */
  const char *allow_env = getenv("X3D_TOGGLE_ALLOW_STDOUT");
  if (allow_env && strcmp(allow_env, "1") == 0) {
    allow_stdout = 1;
  }

  /* If not explicitly allowed, only permit stdout when not running under pkexec/sudo */
  if (!allow_stdout) {
    if (getenv("PKEXEC_UID") == NULL && getenv("SUDO_USER") == NULL) {
      allow_stdout = 1;
    }
  }

  if (strcmp(mode, "stdout") == 0 && allow_stdout) {
    if (priority == LOG_ERR || priority == LOG_WARNING) {
      fprintf(stderr, "%s\n", msgbuf);
    } else {
      printf("%s\n", msgbuf);
    }
  }
}

static int check_compute_load(int threshold, int sample_ms) {
  unsigned long long idle1, total1, idle2, total2;

  total1 = get_cpu_stats(&idle1);
  usleep(sample_ms * 1000);
  total2 = get_cpu_stats(&idle2);

  if (total2 <= total1)
    return 0;

  unsigned long long total_delta = total2 - total1;
  unsigned long long idle_delta = idle2 - idle1;

  unsigned long long usage_pct =
      (100 * (total_delta - idle_delta)) / total_delta;

  if (usage_pct >= (unsigned int)threshold) {
    return 1; /* True, load is high */
  }
  return 0; /* False */
}

static int get_mode() {
  char *target_path = find_x3d_mode_file();
  if (!target_path) {
    printf("unknown\n");
    return 1;
  }

  FILE *fp = fopen(target_path, "r");
  if (!fp) {
    printf("unknown\n");
    free(target_path);
    return 1;
  }

  char buffer[64];
  if (fgets(buffer, sizeof(buffer), fp)) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
      buffer[len - 1] = '\0';
    printf("%s\n", buffer);
  } else {
    printf("unknown\n");
  }

  fclose(fp);
  free(target_path);
  return 0;
}

static int apply_mode(const char *mode) {
  if (!check_root())
    return 0;

  char *target_path = find_x3d_mode_file();
  if (!target_path) {
    audit_log(LOG_ERR, "Could not find sysfs node at %s", AMD_X3D_MODE_GLOB);
    return 0;
  }

  FILE *fp = fopen(target_path, "w");
  if (!fp) {
    audit_log(LOG_ERR, "Could not open %s for writing: %s", target_path, strerror(errno));
    free(target_path);
    return 0;
  }

  size_t to_write = strlen(mode);
  size_t written = fwrite(mode, 1, to_write, fp);
  if (written != to_write) {
    audit_log(LOG_ERR, "Failed to write to %s: %s", target_path, strerror(errno));
    fclose(fp);
    free(target_path);
    return 0;
  }

  if (fclose(fp) != 0) {
    audit_log(LOG_ERR, "Failed to apply mode (device driver rejected?): %s", strerror(errno));
    free(target_path);
    return 0;
  }

  audit_log(LOG_INFO, "Successfully set mode to '%s' (invoker=%s)", mode, getenv("SUDO_USER") ? getenv("SUDO_USER") : (getenv("PKEXEC_UID") ? getenv("PKEXEC_UID") : "unknown"));
  printf("Successfully set mode to '%s'.\n", mode);
  free(target_path);
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    /* GUI launch fallback when run with no arguments */
    if (system("command -v x3d-toggle-gui >/dev/null 2>&1") == 0) {
      execlp("x3d-toggle-gui", "x3d-toggle-gui", NULL);
      /* If execlp fails, fall through and print usage */
    }
    print_usage(argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 ||
      strcmp(argv[1], "-h") == 0) {
    print_usage(argv[0]);
    return 0;
  }

  if (strcmp(argv[1], "check-load") == 0) {
    int threshold = 50;
    if (argc > 2) {
      char *endptr = NULL;
      long parsed = strtol(argv[2], &endptr, 10);
      if (endptr == argv[2] || *endptr != '\0' || parsed < 0 || parsed > 100) {
        fputs("Error: threshold must be an integer between 0 and 100.\n", stderr);
        return 1;
      }
      threshold = (int)parsed;
    }
    if (check_compute_load(threshold, 200)) {
      return 0; /* Success (Load is high) */
    } else {
      return 1; /* Failure (Load is low) */
    }
  }

  if (strcmp(argv[1], "get") == 0) {
    return get_mode();
  }

  if (strcmp(argv[1], "log-on") == 0) {
    const char *config_path = (argc > 2) ? argv[2] : "/etc/x3d-toggle.conf";
    int r = write_config_atomic(config_path, "logmode", "stdout");
    return r == 0 ? 0 : 1;
  }

  if (strcmp(argv[1], "log-off") == 0) {
    const char *config_path = (argc > 2) ? argv[2] : "/etc/x3d-toggle.conf";
    int r = write_config_atomic(config_path, "logmode", "none");
    return r == 0 ? 0 : 1;
  }

  if (strcmp(argv[1], "log-file") == 0) {
    const char *config_path = (argc > 2) ? argv[2] : "/etc/x3d-toggle.conf";
    int r = write_config_atomic(config_path, "logmode", "file");
    return r == 0 ? 0 : 1;
  }

  if (strcmp(argv[1], "set") == 0) {
    /* set <key> <value> [config_path] */
    if (argc < 4) {
      fputs("Usage: set <key> <value> [config_path]\n", stderr);
      return 1;
    }
    const char *key = argv[2];
    const char *value = argv[3];
    const char *config_path = (argc > 4) ? argv[4] : "/etc/x3d-toggle.conf";

    /* Basic validation for supported keys */
    if (strcmp(key, "polling") == 0) {
      long v = strtol(value, NULL, 10);
      if (v < 1 || v > 3600) {
        fputs("Error: polling must be 1..3600\n", stderr);
        return 1;
      }
    } else if (strcmp(key, "compute") == 0) {
      long v = strtol(value, NULL, 10);
      if (v < 1 || v > 100) {
        fputs("Error: compute must be 1..100\n", stderr);
        return 1;
      }
    } else if (strcmp(key, "game") == 0) {
      long v = strtol(value, NULL, 10);
      if (v < 1 || v > 3) {
        fputs("Error: game must be 1..3\n", stderr);
        return 1;
      }
    } else if (strcmp(key, "logmode") == 0) {
      if (!(strcmp(value, "stdout") == 0 || strcmp(value, "file") == 0 || strcmp(value, "none") == 0)) {
        fputs("Error: logmode must be one of: stdout,file,none\n", stderr);
        return 1;
      }
    } else {
      fputs("Error: unsupported key\n", stderr);
      return 1;
    }

    return write_config_atomic(config_path, key, value);
  }

  const char *mode = NULL;
  if (strcmp(argv[1], "cache") == 0) {
    mode = "cache";
  } else if (strcmp(argv[1], "frequency") == 0 ||
             strcmp(argv[1], "freq") == 0) {
    mode = "frequency";
  } else if (strcmp(argv[1], "auto") == 0) {
    /* Map "auto" placeholder to "frequency" for sysfs compatibility */
    mode = "frequency";
  } else if (strcmp(argv[1], "stress") == 0) {
    long stress_time = 10;
    if (argc > 2) {
      char *endptr = NULL;
      long parsed = strtol(argv[2], &endptr, 10);
      if (endptr == argv[2] || *endptr != '\0' || parsed < 1 || parsed > 300) {
        fputs("Error: stress time must be an integer between 1 and 300 seconds.\n", stderr);
        return 1;
      }
      stress_time = parsed;
    }
    printf("Starting %ld second CPU stress test (all cores).\n", stress_time);
    
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "for i in $(seq 1 $(nproc)); do (while true; do :; done) & done; sleep %ld; kill -9 $(jobs -p) 2>/dev/null || true", stress_time);
    
    if (system(cmd) != 0) {
      return 1;
    }
    printf("Stress test complete.\n");
    return 0;
  } else {
    write(STDERR_FILENO, "Unknown command: ", 17);
    write(STDERR_FILENO, argv[1], strlen(argv[1]));
    write(STDERR_FILENO, "\n", 1);
    print_usage(argv[0]);
    return 1;
  }

  if (apply_mode(mode)) {
    return 0;
  } else {
    return 1;
  }
}
