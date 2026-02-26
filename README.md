# x3d-toggle
### X3D-Control v0.6.2_beta - README.md
### Copyright (C) 2026 Pyrotiger

## AMD 3D v-Cache Technology Toggle Control - Community Edition
A portable, high-performance utility for managing CCD priority on the AMD Ryzen X3D CPUs utilizing vCache technology under Linux via manual control or automated daemon.

### Overview
This utility provides a graphical interface to the amd-x3d-vcache kernel driver. It allows for real-time manual switching of the CPU scheduler bias to optimize for specific workloads on asymmetric dual-CCD processors via a simple kdialog. It does this by overriding the global hardware state to resolve the asymmetric scheduling gap between CCDs, eliminating the micro-stuttering that current CPPC drivers fail to address.

* The Problem: CPPC Latency & Asymmetry
Modern Linux kernels often struggle with the "Dual-CCD Paradox." On processors like the 9950X3D, the scheduler must decide between:

CCD0 (V-Cache): High-impact gaming performance.

CCD1 (Frequency): High-clock compute performance.

Current CPPC (Collaborative Processor Performance Control) drivers often fail to switch these states deterministically, leading to micro-stutters as threads "bounce" between cache and frequency pools.

* The Solution: x3d-toggle
This utility interacts directly with the sysfs platform driver to force a global hardware state.

Manual Mode: Instant, user-defined priority.

Automated Daemon: Real-time heuristics that detect "Gaming" vs "Compute" loads and shift the entire hardware topology to match the workload before the stutter occurs.

* Hardware Modes
1. Rabbit Mode (vCache / CCD0): Prioritizes the CCD equipped with 3D V-Cache. Optimized for gaming and latency-sensitive applications.
2. Cheetah Mode (Frequency / CCD1): Prioritizes the high-frequency CCD. Optimized for local LLM inference, compilation, and raw compute.
3. Default Scheduler: Defaults to the system driver for dynamic scheduling, conditional on kernel support. Default is CPPC bias scheduler. Disables the automation daemon.
4. Restart Daemon: Restarts the daemon and persists next boot

### Prerequisites
* UEFI Configuration: CPPC Dynamic Preferred Cores must be set to [Driver].
* System Dependencies: kdialog, bc, polkit, procps-ng and libnotify.

### Security & Logic
* The application interfaces with the sysfs node at /sys/devices/platform/AMDI*/amd_x3d_mode utilizing a dedicated PolicyKit (Polkit) architecture. To ensure strict security and a seamless, password-free user experience, hardware writes are isolated to a containerized root helper script (/usr/libexec/x3d-apply). The sysfs kernel node remains strictly locked to root:root, while the helper script explicitly sanitizes and validates all inputs before securely executing the state change via pkexec. This ensures a seamless UX experience for the user.

### Automation Daemon
* This backend interface dynamically switches the X3D v-Cache mode between Cache, Frequency, and Auto (Driver default). It is built to detect gaming workloads via the following: active game thread, active Steam layer, and Gamemode status. It utilizes system load average to trigger Frequency mode for heavy compute tasks, and defaults to Auto mode for standard desktop usage to maximize efficiency. 

* Work is being done for the next version iteration with improved detection heuristics bringing it to parity with the AMD v-Cache Technology Optimizer driver.

### Installation & Integration
* The utility is installed system-wide. `x3d-control` will launch the application. To integrate the script into your Application Launcher or Dashboard, execute the installer and select 'y'. To manually enable it after installation, run this command in your terminal:

```bash
systemctl --user enable --now x3d-auto
```

## Arch Linux / Garuda Linux (Recommended)
#x3d-toggle-git
* For systems utilizing the `pacman` package manager, deploying via the included `PKGBUILD` is the optimal method. This compiles the utility into a native `x3d-toggle-git` package, automatically resolves dependencies, and seamlessly integrates the application launcher shortcut into your desktop environment.

**Deployment Execution:**

```bash
git clone [https://github.com/pyrotiger/x3d-toggle.git](https://github.com/pyrotiger/x3d-toggle.git)
cd x3d-toggle
makepkg -si
```
## Manual Installation
```bash
cd to /path/to/folder/x3d-toggle
chmod +x install.sh
sudo ./install.sh
```

* Search for "X3D CCD Control" in your launcher to engage or launch directly from terminal by typing `x3d-control`. Accepted keywords are x3d, vcache, cpu, rabbit, cheetah, llm, encode, streaming, workload and compute.

### Notification Customization
You can change the notification icon by dropping a .jpeg picture in /usr/share/x3d-toggle and renaming it to ryzen.jpeg, replacing the one in the directory.

### Uninstallation
To uninstall all binaries and assets, run the following:

```bash
cd to /path/to/folder/x3d-toggle
chmod +x uninstall.sh
sudo ./uninstall.sh
```

### Legal Disclaimer & Liability Limitation

**USE AT YOUR OWN RISK.** This utility interfaces directly with the Linux kernel and hardware sysfs nodes to modify CPU core scheduling and cache prioritization. By executing these scripts, deploying this package, or utilizing this software in any capacity, you acknowledge and agree to the following:

* **Warranty Voidance:** Manipulating CPU hardware states, frequencies, or standard driver behaviors outside of default operational parameters may void your processor or motherboard manufacturer warranties.
* **No Warranty:** This software is provided "as is," without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. 
* **Indemnification:** The author and contributors shall not be held liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, hardware degradation, catastrophic system failure, data loss, or thermal issues) arising in any way out of the use of this software, even if advised of the possibility of such damage.
* **Trademark:** AMD, Ryzen, and 3D V-Cache are trademarks of Advanced Micro Devices, Inc. This project is an independent community-led utility and is not affiliated with, endorsed by, or sponsored by AMD.

Live Long and Prosper

=/\\=
