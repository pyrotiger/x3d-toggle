# x3d-toggle
## AMD 3D v-Cache Technology Toggle Control - Community Edition
A portable, high-performance utility for managing CCD priority on the AMD Ryzen 9 X3D CPUs utilizing vCache technology under Linux.

## Overview
This utility provides a graphical interface to the amd-x3d-vcache kernel driver. It allows for real-time manual switching of the CPU scheduler bias to optimize for specific workloads on asymmetric dual-CCD processors via a simple kdialog.

Hardware Modes
1. Rabbit Mode (vCache / CCD0): Prioritizes the CCD equipped with 3D V-Cache. Optimized for gaming and latency-sensitive applications.

2. Cheetah Mode (Frequency / CCD1): Prioritizes the high-frequency CCD. Optimized for local LLM inference, compilation, and raw compute.

3. Auto: Relies on the system driver for dynamic scheduling, conditional on kernel support.

## Prerequisites
UEFI Configuration: CPPC Dynamic Preferred Cores must be set to [Driver].

System Dependencies: kdialog, polkit (pkexec), and libnotify.

## Installation & Integration
The utility is installed system-wide. x3d_control will launch the application. To integrate the script into your Application Launcher or Dashboard, execute the installer.
### Arch Linux / Garuda Linux (Recommended)

For systems utilizing the `pacman` package manager, deploying via the included `PKGBUILD` is the optimal method. This compiles the utility into a native `x3d-toggle-git` package, automatically resolves dependencies, and seamlessly integrates the application launcher shortcut into your desktop environment.

**Deployment Execution:**

## bash
git clone [https://github.com/pyrotiger/x3d-toggle.git](https://github.com/pyrotiger/x3d-toggle.git)
cd x3d-toggle
makepkg -si

## bash
cd to /path/to/folder
chmod +x install.sh
sudo ./install.sh

Search for "X3D CCD Control" in your launcher to engage or launch directly from terminal by typing x3d-control.

## Security & Logic
The script interfaces directly with the sysfs node at /sys/devices/platform/AMDI*/amd_x3d_mode. It utilizes pkexec for scoped, secure hardware writes.

## Notification Customization
You can change the notification icon by dropping a .jpeg picture in ~/usr/share/x3d-toggle and renaming it to ryzen.jpeg, replacing the one in the directory.

## Uninstallation
To uninstall all binaries and assets, run the following:

## bash
cd to /path/to/folder
chmod +x uninstall.sh
sudo ./uninstall.sh

## Legal Disclaimer & Liability Limitation

**USE AT YOUR OWN RISK.** This utility interfaces directly with the Linux kernel and hardware sysfs nodes to modify CPU core scheduling and cache prioritization. By executing these scripts, deploying this package, or utilizing this software in any capacity, you acknowledge and agree to the following:

* **Warranty Voidance:** Manipulating CPU hardware states, frequencies, or standard driver behaviors outside of default operational parameters may void your processor or motherboard manufacturer warranties.
* **No Warranty:** This software is provided "as is," without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. 
* **Indemnification:** The author and contributors shall not be held liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, hardware degradation, catastrophic system failure, data loss, or thermal issues) arising in any way out of the use of this software, even if advised of the possibility of such damage.

Live Long and Prosper

=/\=
