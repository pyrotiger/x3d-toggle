#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ftw.h>
#include <errno.h>

#define VERSION "1.0.0"
#define AMD_X3D_MODE_FILENAME "amd_x3d_mode"
#define SYS_PLATFORM_DIR "/sys/devices/platform"

static char *found_path = NULL;

void print_usage(const char *prog_name) {
    printf("x3d-toggle-c v%s\n", VERSION);
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

int find_file_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void)sb; (void)typeflag; (void)ftwbuf; // Unused
    if (strstr(fpath, AMD_X3D_MODE_FILENAME) != NULL) {
        // Double check filename match exactly at the end
        const char *filename = strrchr(fpath, '/');
        if (filename && strcmp(filename + 1, AMD_X3D_MODE_FILENAME) == 0) {
            found_path = strdup(fpath);
            return 1; // Stop searching
        }
    }
    return 0;
}

char *find_x3d_mode_file() {
    found_path = NULL;
    if (nftw(SYS_PLATFORM_DIR, find_file_cb, 20, FTW_PHYS) == -1) {
        return NULL;
    }
    return found_path;
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

    char cpu[5];
    unsigned long long user, nice, system, idle_ticks, iowait, irq, softirq, steal;
    
    user = nice = system = idle_ticks = iowait = irq = softirq = steal = 0;

    sscanf(buffer, "%s %llu %llu %llu %llu %llu %llu %llu %llu", 
           cpu, &user, &nice, &system, &idle_ticks, &iowait, &irq, &softirq, &steal);

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
        return 1; // True, load is high
    }
    return 0; // False
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
        fprintf(stderr, "Error: Could not find %s file in %s\n", AMD_X3D_MODE_FILENAME, SYS_PLATFORM_DIR);
        return 0;
    }

    FILE *fp = fopen(target_path, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open %s for writing: %s\n", target_path, strerror(errno));
        free(target_path);
        return 0;
    }

    fprintf(fp, "%s", mode);
    fclose(fp);
    
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
            return 0; // Success (Load is high)
        } else {
            return 1; // Failure (Load is low)
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
