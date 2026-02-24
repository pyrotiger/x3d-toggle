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

## Bash
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

## Bash
cd to /path/to/folder
chmod +x uninstall.sh
sudo ./uninstall.sh

Live Long and Prosper

=/\=
