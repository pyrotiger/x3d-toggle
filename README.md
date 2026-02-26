# x3d-toggle
### X3D-Control v0.6.2_beta - README.md
### Copyright ©️ 2026 Pyrotiger

## AMD 3D v-Cache Technology Toggle Control - Community Edition
A portable, high-performance utility for managing CCD priority on the AMD Ryzen X3D CPUs utilizing vCache technology under Linux via manual control or automated daemon.

### 🔭  Overview  🔭
This utility provides a graphical interface to the amd-x3d-vcache kernel driver. It allows for real-time manual switching of the CPU scheduler bias to optimize for specific workloads on asymmetric dual-CCD processors via a simple kdialog. It does this by overriding the global hardware state to resolve the asymmetric scheduling gap between CCDs, eliminating the micro-stuttering that current CPPC drivers fail to address.

* The Problem: CPPC Latency & Asymmetry
Modern Linux kernels often struggle with the "Dual-CCD Paradox." On processors like the 9950X3D, the scheduler must decide between:

  * CCD0 (V-Cache): High-impact gaming performance. 🐰

  * CCD1 (Frequency): High-clock compute performance. 🐯

  Current CPPC (Collaborative Processor Performance Control) drivers often fail to switch these states deterministically, leading to micro-stutters as threads "bounce" between cache and frequency pools.

* The Solution: x3d-toggle - This utility interacts directly with the sysfs platform driver to force a global hardware state.

  * Automated Daemon: Real-time heuristics that detect "Gaming" vs "Compute" loads and shift the entire hardware topology to match the workload before the stutter occurs.

  * Hardware/Manual Modes - Instant, user-defined priority.
    * Rabbit Mode 🐰 (vCache / CCD0): Prioritizes the CCD equipped with 3D V-Cache. Optimized for gaming and latency-sensitive applications.
    * Cheetah Mode 🐯 (Frequency / CCD1): Prioritizes the high-frequency CCD. Optimized for local LLM inference, compilation, and raw compute.
    * Default Scheduler: Defaults to the system driver for dynamic scheduling, conditional on kernel support. Default is CPPC bias scheduler. Disables the automation daemon.
    * Restart Daemon: Restarts the daemon and persists next boot

### 📜  Prerequisites  📜
* UEFI Configuration: CPPC Dynamic Preferred Cores must be set to [Driver].
* System Dependencies: kdialog, bc, polkit, procps-ng and libnotify.

### 🛡️  Architecture & Security   🛡️
The utility interfaces with the sysfs node at /sys/devices/platform/AMDI*/amd_x3d_mode via a dedicated PolicyKit (Polkit) architecture.
* Isolation: Hardware writes are restricted to a containerized root helper script (/usr/libexec/x3d-apply). 🔒
* Permissions: The sysfs kernel node remains locked to root:root. 🔒
* Validation: The helper script sanitizes all inputs before executing state changes via pkexec, ensuring a secure, password-free user experience. 🔒

### 🛰️⚡  Automation Daemon (x3d-auto)  ⚡🤖
The backend daemon dynamically toggles between Cache, Frequency, and Auto (Driver default) modes by monitoring:
* Gaming Signatures: Active game threads, Steam overlay layers, and gamemode status.
* Compute Loads: System load averages trigger Frequency mode for heavy multi-threaded tasks.
* Efficiency: Defaults to Auto for standard desktop workloads.

>[!TIP]
>Development is underway to achieve parity with the AMD v-Cache Technology Optimizer driver through improved detection heuristics.

### 📦  Installation & Integration  📦
>[!TIP]
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
  * Path: /usr/share/x3d-toggle/ryzen.jpeg
  * Requirement: Image must be named ryzen.jpeg and in jpeg format

### 🚮  Uninstallation  🚮
To uninstall all binaries and assets, run the following:

```bash
cd to /path/to/folder/x3d-toggle
chmod +x uninstall.sh
sudo ./uninstall.sh
```

### ⚖️  Legal Disclaimer & Liability Limitation  ⚖️

**USE AT YOUR OWN RISK.** This utility interfaces directly with the Linux kernel and hardware sysfs nodes to modify CPU core scheduling and cache prioritization. By executing these scripts, deploying this package, or utilizing this software in any capacity, you acknowledge and agree to the following:

* **Warranty Voidance:** Manipulating CPU hardware states, frequencies, or standard driver behaviors outside of default operational parameters may void your processor or motherboard manufacturer warranties.
* **Liability:** The author and contributors shall not be held liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, hardware degradation, catastrophic system failure, data loss, or thermal issues) arising in any way out of the use of this software, even if advised of the possibility of such damage. The software is provided "as is" without any implied warranty.
* **Trademark:** AMD, Ryzen, and 3D V-Cache are trademarks of Advanced Micro Devices, Inc. This project is an independent community-led utility and is not affiliated with, endorsed by, or sponsored by AMD.

Live Long and Prosper

=/\\=
