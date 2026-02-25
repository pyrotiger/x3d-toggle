# Contributing to X3D-Control

## 1. Architectural Objective
Contributions must integrate seamlessly with the established PolicyKit (Polkit) security framework. Hardware state modifications must remain isolated to the containerized root helper (`x3d-apply`). Submissions that compromise the password-free execution flow or bypass sysfs input sanitization will be rejected.

## 2. Development & Testing Baseline
* **Target Hardware:** Validation requires an asymmetric dual-CCD AMD Ryzen processor utilizing 3D V-Cache (e.g., Ryzen 9 9950X3D).
* **Target Environment:** Native compilation and testing should utilize Arch/Garuda Linux via `makepkg -si`.
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
2.  Provide terminal outputs confirming script dependency checks (`kdialog`, `bc`, `gamemoded`) pass successfully on initialization.
3.  If the patch alters operational behavior, include the exit status logs of the Polkit helper executing the state change.
4.  Maintain version parity. If the commit advances the release, update version string headers across all bash files, `README.md`, and the `PKGBUILD` array.