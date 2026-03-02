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
- **Tool Selection:** STRICTLY rely on VS Code native AI tools (`read_file` and `replace_string_in_file`) for file investigation and modification. The host terminal utilizes aliased pagers (`bat`, `less`) which will corrupt or swallow standard CLI pipelines (like `cat`, `sed`, `git diff`) and cause hallucinated states. DO NOT edit files via terminal scripts.
- **Logic:** Use top-down deductive reasoning (Sherlock).
- **Updates:** Perform full-file overwrites or rely on native API editing tools only.