# Copilot Instructions for x3d-toggle

## Architecture & Big Picture
- **Service Boundaries:** The project is split into a high-performance C backend (`x3d-toggle.c`) and a Bash-based heuristic daemon (`x3d-daemon`). 
- **The C Binary:** Acts as a secure, fast proxy to write to the kernel sysfs node (`/sys/devices/platform/AMDI*/amd_x3d_mode`). It also handles performant CPU utilization polling (`x3d-toggle check-load`).
- **The Daemon:** Runs as a systemd user service (`x3d-auto.service`). It evaluates gaming intent (via Steam, gamemode, desktop files) or compute load, then invokes the C binary via `pkexec`.
- **Terminology:** 
  - 🐰 **Rabbit Mode:** `cache` (prioritizes 3D V-Cache for gaming).
  - 🐆 **Cheetah Mode:** `frequency` (prioritizes high clocks for compute tasks).
  - 🦌 **Elk Mode:** `auto` (returns scheduling to the default kernel driver).

## Security & Permissions
- Never alter the permissions strategy. Hardware writes require root privileges. 
- The project **strictly uses PolicyKit** (`org.x3dtoggle.policy`) for privilege elevation. Do not implement `setuid` bits or raw `sudo` commands inside the user-facing GUI/daemon. Always use `pkexec /usr/bin/x3d-toggle` when invoking from user-space scripts.
- The C binary enforces safety by checking `geteuid() == 0` securely.

## Agent Tool Use & File Manipulation
- **File Editing Strictness:** Do NOT use chained bash commands (`cat | tail > file`), complex redirection sequences, or blind shell text splicing to try and update files (especially large documents like `CHANGELOG.md`). This leads to repetitive file corruption over long development sessions.
- **Native Context Tools:** Always default to using reliable native editing tools (`replace_string_in_file`) for surgical edits.
- **Direct Overwrites:** If making massive changes or if native editing tools fail, do not overcomplicate text manipulation. Use a single, clean overwrite (e.g. providing the full target state or using a clean `cat << 'EOF' > file` terminal heredoc) rather than risky partial CLI patching.

## Developer Workflows & Build Commands
 ```bash
 ```
- **C Code (`x3d-toggle.c`):** Use minimal standard libraries (`stdio.h`, `glob.h`, `unistd.h`). Avoid introducing heavyweight dependencies (like glib) to keep latency near zero. Optimize file reads when parsing `/proc/stat`.
- **GUI:** The GUI is implemented via `kdialog` (`x3d-toggle-gui`). Keep dialogs simple and native to KDE/Plasma conventions, but functional in other desktop environments.

## Development & Test Environment
Always contextualize suggestions, hardware-specific paths, and troubleshooting based on the user's primary workstation:
- **OS:** Garuda Linux x86_64 (`pacman` package manager)
- **Kernel:** Linux 7.0.0-rc1-1-cachyos-rc (custom/optimized kernel)
- **CPU:** AMD Ryzen 9 9950X3D (32 threads) @ 5.80 GHz (Target Hardware)
- **GPU:** AMD Radeon RX 7900 XTX [Discrete] (Mesa / radv)
- **Display Server / DE:** Wayland (KWin) within KDE Plasma 6.6.1
- **Shell / Terminal:** fish 4.5.0 / Konsole 25.12.2
