#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
#include <errno.h>

#define VERSION "1.0.4"
#define AMD_X3D_MODE_GLOB "/sys/devices/platform/AMDI*/amd_x3d_mode"

void print_usage(const char *prog_name) {
    printf("x3d-toggle v%s\n", VERSION);
    printf("Usage: %s <command> [args]\n", prog_name);
    printf("Commands:\n");
    printf("  cache        Enable 3D V-Cache preference (Rabbit Mode)\n");
    printf("  frequency    Enable Frequency preference (Cheetah Mode)\n");
    printf("  auto         Reset to OS default behavior (Elk Mode)\n");
    printf("  get          Print current mode\n");
    printf("  check-load   Check if CPU usage exceeds threshold (Return 0 if yes)\n");
    printf("               Arg: <threshold_percent> (default: 50)\n");
    printf("  help         Show this help message\n");
}

int check_root() {
    if (geteuid() != 0) {
        fprintf(stderr, "Error: This program must be run as root.\n");
        return 0;
    }
    return 1;
}

char *find_x3d_mode_file() {
    glob_t globbuf;
    char *result_path = NULL;

    if (glob(AMD_X3D_MODE_GLOB, GLOB_NOSORT, NULL, &globbuf) == 0) {
        if (globbuf.gl_pathc > 0) {
            result_path = strdup(globbuf.gl_pathv[0]);
        }
    }
    globfree(&globbuf);
    
    return result_path;
}

unsigned long long get_cpu_stats(unsigned long long *idle) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 0;
    
    char buffer[1024];
    if (!fgets(buffer, sizeof(buffer), fp)) {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    unsigned long long user, nice, system, idle_ticks, iowait, irq, softirq, steal;
    
    user = nice = system = idle_ticks = iowait = irq = softirq = steal = 0;

    sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", 
           &user, &nice, &system, &idle_ticks, &iowait, &irq, &softirq, &steal);

    if (idle) *idle = idle_ticks + iowait; 
    
    return user + nice + system + idle_ticks + iowait + irq + softirq + steal;
}

int check_compute_load(int threshold, int sample_ms) {
    unsigned long long idle1, total1, idle2, total2;
    
    total1 = get_cpu_stats(&idle1);
    usleep(sample_ms * 1000);
    total2 = get_cpu_stats(&idle2);

    if (total2 <= total1) return 0;

    unsigned long long total_delta = total2 - total1;
    unsigned long long idle_delta = idle2 - idle1;
    
    if (total_delta == 0) return 0;

    unsigned long long usage_pct = (100 * (total_delta - idle_delta)) / total_delta;
    
    if (usage_pct >= (unsigned int)threshold) {
        return 1;
    }
    return 0;
}

int get_mode() {
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
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
    } else {
        printf("unknown\n");
    }

    fclose(fp);
    free(target_path);
    return 0;
}

int apply_mode(const char *mode) {
    if (!check_root()) return 0;

    char *target_path = find_x3d_mode_file();
    if (!target_path) {
        fprintf(stderr, "Error: Could not find sysfs node at %s\n", AMD_X3D_MODE_GLOB);
        return 0;
    }

    FILE *fp = fopen(target_path, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open %s for writing: %s\n", target_path, strerror(errno));
        free(target_path);
        return 0;
    }

    if (fprintf(fp, "%s", mode) < 0) {
        fprintf(stderr, "Error: Failed to write to %s: %s\n", target_path, strerror(errno));
        fclose(fp);
        free(target_path);
        return 0;
    }
    
    if (fclose(fp) != 0) {
        fprintf(stderr, "Error: Failed to apply mode (device driver rejected?): %s\n", strerror(errno));
        free(target_path);
        return 0;
    }
    
    printf("Successfully set mode to '%s'.\n", mode);
    free(target_path);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "check-load") == 0) {
        int threshold = 50;
        if (argc > 2) {
            char *endptr = NULL;
            long parsed = strtol(argv[2], &endptr, 10);
            if (endptr == argv[2] || *endptr != '\0' || parsed < 0 || parsed > 100) {
                fprintf(stderr, "Error: threshold must be an integer between 0 and 100.\n");
                return 1;
            }
            threshold = (int)parsed;
        }
        if (check_compute_load(threshold, 200)) {
            return 0;
        } else {
            return 1;
        }
    }

    if (strcmp(argv[1], "get") == 0) {
        return get_mode();
    }

    const char *mode = NULL;
    if (strcmp(argv[1], "cache") == 0) {
        mode = "cache";
    } else if (strcmp(argv[1], "frequency") == 0 || strcmp(argv[1], "freq") == 0) { 
        mode = "frequency";
    } else if (strcmp(argv[1], "auto") == 0) {

        mode = "auto";
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }

    if (apply_mode(mode)) {
        return 0;
    } else {
        return 1;
    }
}
