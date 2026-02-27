# v1.0.0 - The C Refactor Release 🚀

This major release marks a complete architectural rewrite of the core logic, transitioning from Bash scripts to a high-performance **C binary backend**. This ensures instant execution, minimal system overhead, and robust hardware interaction.

## 🔥 Key Highlights
*   **Performance Rewrite:** The core hardware interaction logic (`x3d-apply`) has been replaced by a compiled C binary (`x3d-toggle-c`). This eliminates shell interpretation overhead for critical hardware state changes.
*   **Native File Operations:** Replaced external shell commands (`find`, `cat`, `awk`) with native C system calls (`nftw`, `fopen`) for faster and safer file handling.
*   **Smart Load Detection:** The daemon now uses a precise **CPU Usage Percentage** threshold (via `/proc/stat` polling in C) instead of the older "load average" metric, providing more accurate and responsive compute-mode switching.

## 🛠️ Technical Changes
*   **New Binary:** `x3d-toggle-c` handles all hardware writes, state queries (`get`), and load monitoring (`check-load`).
*   **Updated Scripts:** `x3d-daemon` and `x3d-control` have been updated to act as lightweight wrappers around the C binary.
*   **Configuration:** `COMPUTE_LOAD_THRESHOLD` now accepts an integer percentage (Default: `50` for 50% CPU usage) instead of a load average float.
*   **Build System:** Added a standard `Makefile` for building and installing the full suite.
*   **Packaging:** Updated `PKGBUILD` to compile from source.

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
