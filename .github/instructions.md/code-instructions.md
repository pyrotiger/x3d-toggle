# Engineering Standards & Security

## eBPF & C Development
- **Ground Truth:** Never assume kernel offsets. Always prioritize the locally generated `vmlinux.h`.
- **Latency Target:** Zero-overhead. Use `BPF_MAP_TYPE_RINGBUF` for event streaming.
- **Dependencies:** Keep C binaries lean. Avoid heavyweight libraries like `glib`.

## Security Protocol
- **Elevation:** Do not use `setuid` or raw `sudo` within scripts.
- **PolicyKit:** Strictly utilize `pkexec` for all sysfs writes via `org.x3dtoggle.policy`.
- **Validation:** C binaries must enforce `geteuid() == 0` securely.

## Agent Behavior
- **Logic:** Use top-down deductive reasoning (Sherlock).
- **Updates:** Perform full-file overwrites or clean heredocs; avoid partial CLI patching.