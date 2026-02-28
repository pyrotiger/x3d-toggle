# v1.0.3 - System Precision & UI Polish Release 🚀

This update focuses on surgical bug fixes, refining the underlying system heuristics, optimizing the kdialog graphical interface, and drastically improving the dynamic logic loops following the transition to the C binary backend. 

## 🛠️ Logic & Hardware Interaction Fixes
* **Driver Standard Baseline Mapping:** The C backend logic now safely translates `auto` parameter flags natively into Linux's baseline `frequency` string state. This eliminates strict `invalid argument` hardware/sysfs rejections across all AMD PMF kernel driver profiles.
* **Game Detection Overhaul:** Rewrote the daemon's internal `check_gaming_intent()` functions. Targeted `pgrep` strictly to `-f steamapps\/common` and enforced whole-word matching (`grep -qw` instead of `grep -q`) on `gamemoted` outputs. This nullifies recurring false-positive locks caused by system kernels like `[oom_reaper]`. 
* **Daemon Hierarchy Overrides:** Manually selecting an execution state via the physical GUI dialogue menu now triggers `systemctl --user stop x3d-auto`, actively pausing the invisible monitoring daemon preventing aggressive recursive CPU state wrestling. 

## 🎨 UI & Environment Layout 
* **KDE / kdialog Grid Parsing:** Eliminated arbitrary parameters (`0 0 0`) from the radiolist syntax fixing a critical cosmetic GUI parser issue resulting in an orphaned numerical '0' row in KDE Plasma 6+.
* **Freedesktop Standard Desktop Icons:** Redesigned `.desktop` execution to natively request the icon via global path associations standardizing icon display structures. Application menus, taskbars, and custom shortcuts now resolve visuals flawlessly without explicit layout pathing errors.

## 📦 Packaging Polish
* **Post Install Alerts:** Created custom ASCII boundaries encapsulating package completion hints ensuring `desktop` instructions are visually legible over `makepkg` transaction floods.

---

# v1.0.2 - Security, Standards, & C Refactor Release 🚀

This major release marks a complete architectural rewrite of the core logic, transitioning from bash scripts to a high-performance **C binary backend**. It has undergone a rigorous security audit and been hardened to meet strict Arch Linux AUR, FHS, and FreeDesktop (KDE/GNOME) packaging standards. This ensures instant execution, minimal system overhead, and robust, safe hardware interaction.

## 🛡️ Security & Auditing (The "Linus-Approved" Hardening)
*   **Buffer Overflow Protections:** Rewrote `/proc/stat` parsing logic. Hardened `sscanf(buffer, "%*s %llu...")` to safely ignore alphanumeric strings via `%*s` instead of utilizing finite char arrays, closing potential stack-smashing vulnerabilities.
*   **Memory & File I/O Safety:** Added strict memory and resource lifecycle management. Ensured all sysfs file mutations explicitly check for `errno` and safely call `fclose()` even during error routines.
*   **Hardened Compiler Targets:** The `Makefile` now strictly enforces `-O2 -Wall -Wextra -Werror -Wformat-security -D_FORTIFY_SOURCE=2` preventing compilation if warnings exist and actively mitigating buffer exploits.
*   **POSIX Path Pathing:** Replaced external shell commands (`find`, `cat`, `awk`) with native C system calls (`nftw`). Fortified the file tree walker utilizing the `FTW_F` macro and native pointer offsets `(fpath + ftwbuf->base)` for direct and heavily optimized `strcmp` string comparisons.
*   **100% Shellcheck Compliance:** All surrounding Bash utility wrappers (`x3d-daemon`, `x3d-toggle-gui`, and `x3d-toggle.install`) now fully pass strict `shellcheck` validation, nullifying quote and variable expansion pitfalls.

## 🏆 Standards & Compliance (FHS / KDE / AUR)
*   **FreeDesktop Standards Validation:** The `x3d-toggle-gui.desktop` file now passes strict `desktop-file-validate` pipelines.
*   **KDE Desktop Embellishments:** Added required FreeDesktop `Keywords=...;elk;` to the `.desktop` manifest, drastically improving native application launcher integration across Plasma and GNOME.
*   **AUR Best Practices:** Dropped the insecure `SKIP` checksum flag. The `PKGBUILD` now perfectly downloads the remote GitHub release tarball and statically validates the `sha256sums` arrays to ensure supply chain integrity via `updpkgsums`.

## 🛠️ Technical Changes
*   **New Binary:** `x3d-toggle` handles all hardware writes, state queries (`get`), and load monitoring (`check-load`).
*   **Updated Scripts:** `x3d-daemon` and `x3d-control` have been updated to act as lightweight wrappers around the C binary.
*   **Configuration:** `COMPUTE_LOAD_THRESHOLD` now accepts an integer percentage (Default: `50` for 50% CPU usage) instead of a load average float.
*   **Build System:** Added a standard `Makefile` for building and installing the full suite.
*   **Packaging:** Updated `PKGBUILD` to compile from source.
*   **Security:** Hardened security protocols and added buffer overflow protections

## 🗑️ Removed
*   Removed `x3d-apply` (Bash implementation).
*   Removed `install.sh` and `uninstall.sh` (Replaced by `make install`).

## 📦 Installation
**Arch Linux:**
```bash
makepkg -si
```
**Manual:**
```bash
make
sudo make install
```
