# x3d-toggle (C Implementation)
### X3D-Toggle v1.2.0 - README.md
### Copyright ©️ 2026 Pyrotiger - License: GPLv3

## AMD 3D V-Cache Performance Optimizer Toggle Control - Community Edition
A portable, high-performance utility for managing CCD priority on the AMD Ryzen X3D CPUs utilizing the 3D V-Cache Performance Optimizer under Linux via manual control or automated daemon.

### 🔭  Overview  🔭
This utility provides a graphical interface and automation to the `amd-x3d-vcache` kernel driver. It allows for real-time switching of the CPU scheduler bias to optimize for specific workloads on asymmetric dual-CCD processors (like 7950X3D/9950X3D).

* The Problem: CPPC Latency & Asymmetry
  Current CPPC drivers often fail to switch states deterministically, leading to micro-stutters.

* The Solution: x3d-toggle
  * **C Binary Backend**: The core logic is now a compiled C binary for instant execution and minimal system overhead. Sysfs node detection uses POSIX `glob()` for real-time latency reduction.
  * **Automated Daemon**: Real-time heuristics detect "Gaming" vs "Compute" loads.
  * **Manual Modes**: Instant user-defined priority via GUI or CLI.

### 📜  Prerequisites  📜
* **Linux Kernel**: 6.13 minimum (provides `amd-x3d-vcache` sysfs node); 7.0+ is recommended to take advantage of the daemon's advanced scheduling features (specifically, real-time CPPC dynamic preferred core hinting and zero-latency thread delegation during compute bursts). When coupled with the daemon (and planned eBPF refinements), this brings full feature parity of the Windows AMD 3D V-Cache Performance Optimizer driver to Linux.
* UEFI Configuration: CPPC Dynamic Preferred Cores set to [Driver].
* System Dependencies: `kdialog` (GUI), `polkit`, `libnotify`.
* Build Dependencies: `gcc`, `make`.

### 🛡️  Architecture Security  🛡️
The utility interfaces with the sysfs node at `/sys/devices/platform/AMDI*/amd_x3d_mode` via PolicyKit.
* **C Binary**: Hardware writes are handled by `x3d-toggle` (installed to `/usr/bin`).
* **Polkit**: Actions are authorized via `org.x3dtoggle.policy`.

### 📦  Installation Methods 📦

#### 1. Arch Linux / Garuda (via Pre-compiled Pacman Package)
If you download the compiled Arch package directly from the **Releases** page (`.pkg.tar.zst`), you can install it seamlessly using `pacman` without needing to compile it yourself:
```bash
cd ~/Downloads/
sudo pacman -U x3d-toggle-*.pkg.tar.zst
```

#### 2. Arch Linux / Garuda (via Local PKGBUILD / Git Clone)
For developers or those wanting to compile directly from the absolute latest commits dynamically. The `makepkg` command automatically builds the C binary and wraps it into a `pacman` installation for you:
```bash
git clone https://github.com/pyrotiger/x3d-toggle.git
cd x3d-toggle
makepkg -si
```

#### 3. Manual Build (Make - Debian/Fedora/Ubuntu/Etc)
If you are running a non-Arch distro without `pacman` or `makepkg`:
```bash
git clone https://github.com/pyrotiger/x3d-toggle.git
cd x3d-toggle
make
sudo make install
```

### 🎮  Application Usage  🎮

* Launch the GUI/Interface via your application launcher (search for "X3D CCD Control") or execute via terminal:
  ```bash
  x3d-toggle-gui
  ```
* Accepted Application/Desktop Launcher Keywords: `x3d` `vcache` `cpu` `rabbit` `cheetah` `llm` `encode` `streaming` `workload` `compute` `elk`
* **Note on Desktop Shortcuts:** In Arch Linux, packages securely place their `.desktop` files in your Application Launcher (`/usr/share/applications/`), rather than forcing icons onto your physical Desktop. If you prefer a literal shortcut icon on your Desktop, simply run this command:
  ```bash
  cp /usr/share/applications/x3d-toggle-gui.desktop ~/Desktop/ && chmod +x ~/Desktop/x3d-toggle-gui.desktop
  ```

### ⚙️  Background Daemon & Utilities  ⚙️

#### Automated Scheduling Service 🎹
Enable the user service to allow the background daemon to dynamically switch your system between Cache and Frequency profiles based on live workloads:
```bash
systemctl --user enable --now x3d-auto.service
```

#### Configuration Overrides  🔧
To fine-tune the daemon's behavior, edit the configuration file `/etc/x3d-toggle.conf` (Requires `sudo` to edit):
*  **POLL_INTERVAL:** Adjust how frequently the daemon checks for state changes in seconds (Default: 3).
*  **COMPUTE_LOAD_THRESHOLD:** Set the CPU usage percentage required to trigger Cheetah/Frequency mode (Default: 50).
*  **GAME_DETECTION_MODE:** Select the method used to detect Gaming/Rabbit mode intent (Default: 2).
   *  Mode `1`: Simplified legacy detection (Gamemoded + Steam).
   *  Mode `2`: Advanced dynamic detection. Caches your installed application metadata (`.desktop` fields) and continually matches it against `top` resource utilization and steam usage.

*  **Note:** After editing, restart the daemon to apply changes:
   ```bash
   systemctl --user restart x3d-auto
   ```

#### CLI Administrative Commands ⌨
The core `sysfs` writes are handled silently by the `x3d-toggle` compiled C binary. You can run checks manually:
```bash
sudo x3d-toggle cache                 # Rabbit Mode (Gaming) 🐰
sudo x3d-toggle frequency             # Cheetah Mode (Compute) 🐆
sudo x3d-toggle auto                  # Elk Mode (Driver Default) 🦌
x3d-toggle get                        # Check current active hardware mode 🔎
x3d-toggle check-load <threshold>     # Performant C check against load % 🧠
x3d-toggle stress <seconds>           # Run synthetic multi-core CPU spiker 🧮
```

### ⚙️  Customizations  ⚙️
*  To change the notification icon, replace the existing asset:
   *  Path: /usr/share/x3d-toggle/ryzen.png
   *  Requirement: Image must be named ryzen.png and in png format

### 🚮  Uninstallation  🚮
To uninstall all binaries and assets, run the following:

```bash
cd to /path/to/folder/x3d-toggle
sudo make uninstall
```
### 🤝  Credits & Acknowledgments  🤝
GrandBIRDLizard — For critical technical insights on sysfs node utilization, GPU IRQ management, and architectural guidance. Author of [X3Dctl](https://github.com/GrandBIRDLizard/x3dctl).

### ⚖️  Legal Disclaimer & Liability Limitation  ⚖️

**USE AT YOUR OWN RISK.** This utility interfaces directly with the Linux kernel and hardware sysfs nodes to modify CPU core scheduling and cache prioritization. By executing these scripts, deploying this package, or utilizing this software in any capacity, you acknowledge and agree to the following:

* **Warranty Voidance:** Manipulating CPU hardware states, frequencies, or standard driver behaviors outside of default operational parameters may void your processor or motherboard manufacturer warranties.
* **Liability:** The author and contributors shall not be held liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, hardware degradation, catastrophic system failure, data loss, or thermal issues) arising in any way out of the use of this software, even if advised of the possibility of such damage. The software is provided "as is" without any implied warranty.
* **Trademark:** AMD, Ryzen, and 3D V-Cache are trademarks of Advanced Micro Devices, Inc. This project is an independent community-led utility and is not affiliated with, endorsed by, or sponsored by AMD.
##
### Live Long and Prosper 🖖

### =/\\=
