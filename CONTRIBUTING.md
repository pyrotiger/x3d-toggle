# Contributing to X3D-Control

## 1. Architectural Objective
Contributions must integrate seamlessly with the established PolicyKit (Polkit) security framework. Hardware state modifications are now handled by the **C binary** (`x3d-toggle-c`).

## 2. Development & Testing
* **Target Hardware:** AMD Ryzen 7000/9000 X3D series.
* **Build System:** The project uses a simple `Makefile`.
    * **Build**: `make`
    * **Install**: `sudo make install`
* **Telemetry Verification:** Automation daemon modifications must be stress-tested. Verify compute and gaming workload heuristics by monitoring the systemd journal (`journalctl --user -u x3d-auto -f`). Ensure successful toggles between `cache`, `frequency`, and `auto`.

## 3. Code & UI Standards
* **I/O Mitigation:** Background scripts (`x3d-daemon`) must not generate continuous disk polling. Target variables (e.g., `.desktop` file parameters) must be cached into memory during initialization.
* **Error Handling:** Scripts must not fail silently. Trap execution failures and Polkit exit statuses (126/127) to maintain a clean diagnostic baseline. 
* **Visual Telemetry:** UI dialogues and `notify-send` alerts must strictly adhere to the established KDE Breeze hex codes:
    * Critical data / write failures: `<font color='#ff6d00'>` (Orange)
    * Hardware State / Active Mode: `<font color='#3daee9'>` (Breeze Blue)
    * Daemon Status / Success: `<font color='#1cdc9a'>` (Breeze Green)

## 4. Pull Request Protocol
1.  Apply deductive reasoning to the PR description. Identify the structural vulnerability or logic flaw before detailing the patch.
2.  Provide terminal outputs confirming script dependency checks (`kdialog`, `gamemoded`) pass successfully on initialization.
3.  If the patch alters operational behavior, include the exit status logs of the Polkit helper executing the state change.
4.  Maintain version parity. If the commit advances the release, update version string headers across all bash files, `README.md`, and the `PKGBUILD` array.