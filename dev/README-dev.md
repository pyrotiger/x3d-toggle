> **⚠️  WARNING: USE AT YOUR OWN RISK, READ FIRST!  ⚠️**
> This directory contains development logic, privilege escalation tests (PolicyKit), and sandboxed execution environments intended strictly for testing the `x3d-toggle` utility. Misuse of these scripts natively can alter CPU logic on your host hardware. Proceed with caution...

### 🧰  Developer Tools & Sandbox  🧰
Developers or enthusiasts who want to modify the source can run the dynamic testing suite directly from the source directory. This leverages `bwrap` (Bubblewrap) to create an isolated, read-only root system, preventing test logic from overwriting your physical OS directories:
```bash
./dev/dev-helper.sh install-all
./dev/dev-helper.sh test
```
The standard GUI also detects missing components and provides prompts to pull standard developer suites automatically when the wizard's easter egg is found.

Development PKExec test
=======================

This folder contains helper scripts to test PolicyKit/`pkexec` integration with a
local, development `x3d-toggle` binary without committing/installing upstream files.

Files
- `org.x3dtoggle.policy.dev` : temporary policy file that authorizes `/usr/bin/x3d-toggle.dev`.
- `tmp-dev-install.sh` : installs the dev binary and policy (requires sudo) and runs a pkexec test.
- `tmp-dev-cleanup.sh` : removes installed dev files.

Usage
1. Build the `x3d-toggle` binary in repo root (you already have it at `./x3d-toggle`).
2. Run the installer to copy the binary to `/usr/bin/x3d-toggle.dev` and install policy:

   By default dev helper runs in a sandbox under `dev/sandbox` and does not require sudo.
   To perform a real system install (unsafe), pass `--unsafe` as the first argument.

   ./dev/dev-helper.sh install-all       # sandboxed (recommended)
   ./dev/dev-helper.sh --unsafe install-all   # real system install (requires sudo)

   The script will attempt a `pkexec` call to test writing a sample config value. It will
   prompt for your auth as usual. If the test succeeds, the polkit action will be accepted
   for the dev binary path.

3. When finished, clean up with:

   sudo ./dev/tmp-dev-cleanup.sh

 Notes
- These are development helpers only. Do not ship these files to production as-is.
- By default actions are sandboxed under `dev/sandbox` to avoid touching system paths.
- The unsafe real installer writes to `/usr/bin/x3d-toggle.dev` and `/usr/share/polkit-1/actions/`.

Security guidance
- Do not add setuid binaries; the project uses PolicyKit for privilege elevation.
- Keep the packaged policy least-permissive. Ship `.rules` examples only in `dev/`.

Packaging notes
- Ensure package scripts set file ownership to root:root and modes to 644 for policy
   and manpages. Avoid installing developer `.rules` files by default.
