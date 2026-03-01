# Future Development Roadmap
- **`eBPF`:** Transition to eBPF for better polling
- **`dev/benchmark.sh`:** A profiling suite designed to measure system call overhead `strace`, structural bash memory leaks, and literal microsecond execution timing (`time`) on binary calls to ensure `x3d-toggle` retains absolute zero-overhead presence in the scheduler.

- **`x3d-toggle-dashboard` (Rust/GTK4):** Develop a dedicated, persistently-running desktop front-end (system tray application).
  - Must remain strictly decoupled from the core `x3d-toggle.c` root backend to preserve absolute zero-overhead execution for the daemon itself.
  - Implement real-time asynchronous polling (CPU % by CCD, exact gaming hook status).
  - Built in Rust + GTK4 (or Relm4) rather than Python/PyQt to eliminate Python GC pauses, dependency rot during OS rolling releases, and 100MB+ interpreter RAM bloat.
  - Allows `x3d-toggle-gui` to survive unchanged as the robust, dependency-free UNIX shell option.
