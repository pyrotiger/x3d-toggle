# x3d-toggle (C Implementation)
### X3D-Toggle v1.0.0 - README.md
### Copyright ©️ 2026 Pyrotiger

## AMD 3D v-Cache Technology Toggle Control - Community Edition
A portable, high-performance utility for managing CCD priority on the AMD Ryzen X3D CPUs utilizing vCache technology under Linux via manual control or automated daemon.

**Now refactored into a high-performance C binary (`x3d-toggle-c`) for minimal overhead.**

### 🔭  Overview  🔭
This utility provides a graphical interface and automation to the `amd-x3d-vcache` kernel driver. It allows for real-time switching of the CPU scheduler bias to optimize for specific workloads on asymmetric dual-CCD processors (like 7950X3D/9950X3D).

* The Problem: CPPC Latency & Asymmetry
  Current CPPC drivers often fail to switch states deterministically, leading to micro-stutters.

* The Solution: x3d-toggle
  * **C Binary Backend**: The core logic is now a compiled C binary for instant execution and minimal system overhead.
  * **Automated Daemon**: Real-time heuristics detect "Gaming" vs "Compute" loads.
  * **Manual Modes**: Instant user-defined priority via GUI or CLI.

### 📜  Prerequisites  📜
* UEFI Configuration: CPPC Dynamic Preferred Cores set to [Driver].
* System Dependencies: `kdialog` (GUI), `polkit`, `libnotify`.
* Build Dependencies: `gcc`, `make`.

### 🛡️  Architecture Security   🛡️
The utility interfaces with the sysfs node at `/sys/devices/platform/AMDI*/amd_x3d_mode` via PolicyKit.
* **C Binary**: Hardware writes are handled by `x3d-toggle-c` (installed to `/usr/bin`).
* **Polkit**: Actions are authorized via `org.x3dtoggle.policy`.

### 📦  Installation  📦

#### Arch Linux (PKGBUILD)
```bash
git clone https://github.com/pyrotiger/x3d-toggle.git
cd x3d-toggle
makepkg -si
```

#### Manual Installation (Make)
```bash
git clone https://github.com/pyrotiger/x3d-toggle.git
cd x3d-toggle
make
sudo make install
```

### 🎮 Usage 🎮

#### Command Line
The core logic is handled by `x3d-toggle-c`:
```bash
sudo x3d-toggle-c cache       # Rabbit Mode (Gaming) 🐰
sudo x3d-toggle-c frequency   # Cheetah Mode (Compute) 🐆
sudo x3d-toggle-c auto        # Driver Default
x3d-toggle-c get              # Check current mode
```

#### GUI Control
Run `x3d-toggle-gui` to open the mode selection dialog.

#### Daemon (Automation)
Enable the user service to automatically switch modes:
```bash
systemctl --user enable --now x3d-auto.service
```

### ⚙️ Configuration ⚙️
Edit `/etc/x3d-toggle.conf` to customize polling intervals and load thresholds.

### ⚖️ License ⚖️
GPLv3
>### Arch Linux / Garuda Linux (Recommended)
>## x3d-toggle-git
>* For systems utilizing the `pacman` package manager, deploying via the included `PKGBUILD` is the optimal method. This compiles the utility into a native `x3d-toggle-git` package, automatically resolves dependencies, >and seamlessly integrates the application launcher shortcut into your desktop environment.
>
>**Deployment Execution:**
>
>```bash
>git clone [https://github.com/pyrotiger/x3d-toggle.git](https://github.com/pyrotiger/x3d-toggle.git)
>cd x3d-toggle
>makepkg -si
>```
## 🛠️  Manual Installation  🛠️
```bash
git clone https://github.com/pyrotiger/x3d-toggle.git
cd x3d-toggle
chmod +x install.sh
sudo ./install.sh
```
### 🚀  Usage  🚀
* Launch the GUI/Interface via your application launcher (search for "X3D CCD Control") or execute via terminal:
  ```bash
  x3d-control
  ```
* Accepted Keywords for Launcher: `x3d` `vcache` `cpu` `rabbit` `cheetah` `llm` `encode` `streaming` `workload` `compute`
* If you opted out of the automation daemon initalization during install, you can start the daemon by running this command in your terminal:
  ```bash
  systemctl --user enable --now x3d-auto
  ```
### ⚙️  Customizations  ⚙️
*  To change the notification icon, replace the existing asset:
   *  Path: /usr/share/x3d-toggle/ryzen.jpeg
   *  Requirement: Image must be named ryzen.jpeg and in jpeg format
*  To fine-tune the daemon's behavior, edit the configuration file:
   *  Path: /etc/x3d-toggle.conf (Requires sudo/root to edit)
   *  **Poll Interval:** Adjust how frequently the daemon checks for state changes (Default: 3s).
   *  **Compute Threshold:** Set the load average required to trigger Frequency/Cheetah mode (Default: 8.0).
   *  **Note:** After editing, restart the daemon to apply changes:
      ```bash
      systemctl --user restart x3d-auto
      ```
### 🚮  Uninstallation  🚮
To uninstall all binaries and assets, run the following:

```bash
cd to /path/to/folder/x3d-toggle
chmod +x uninstall.sh
sudo ./uninstall.sh
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
