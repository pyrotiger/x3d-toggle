# x3d-toggle Logic & State Machine

## Operational Modes
- **Rabbit Mode (cache):** Prioritizes 3D V-Cache (Gaming). Writes to `/sys/devices/platform/AMDI*/amd_x3d_mode`.
- **Cheetah Mode (frequency):** Prioritizes high clocks (Compute/LLMs).
- **Elk Mode (auto):** Reverts to default kernel scheduling.

## Architecture
- **Backend:** `x3d-toggle.c` (High-performance sysfs proxy).
- **Daemon:** `x3d-daemon` (Bash-based heuristic evaluator).
- **Service:** `x3d-auto.service` (Systemd user-space).
- **UI:** `kdialog` for native KDE/Plasma interaction.

## eBPF & Development
- **Ground Truth:** Never assume kernel offsets. Always prioritize the locally generated `vmlinux.h`.
- **Latency Target:** Zero-overhead. Use `BPF_MAP_TYPE_RINGBUF` for event streaming.
- **Dependencies:** Keep binaries lean. Avoid heavyweight libraries like `glib`.