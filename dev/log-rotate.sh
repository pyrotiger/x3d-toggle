#!/usr/bin/env bash
# Simple log rotation helper for the project `logs/` directory.
# - Rotates any `*.log` file when it exceeds MAX_BYTES (default 10 MB).
# - Compresses the rotated file with a timestamp suffix and truncates the live log.
# - Keeps the most recent N archives per log file (default 7).

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LOG_DIR="$REPO_ROOT/logs"
MAX_BYTES=${MAX_BYTES:-10485760} # 10 * 1024 * 1024 = 10MB
KEEP=${KEEP:-7}

mkdir -p "$LOG_DIR"

shopt -s nullglob
for logfile in "$LOG_DIR"/*.log; do
  [ -f "$logfile" ] || continue
  # Determine file size
  filesize=$(stat -c%s -- "$logfile" 2>/dev/null || wc -c <"$logfile")
  if [ "$filesize" -ge "$MAX_BYTES" ]; then
    ts=$(date +%Y%m%d%H%M%S)
    archive="$logfile.$ts.gz"
    # Compress current log into archive, then truncate original
    gzip -c -- "$logfile" >"$archive"
    : >"$logfile"

    # Prune older archives for this base name, keep newest $KEEP
    base=$(basename -- "$logfile")
    dir=$(dirname -- "$logfile")
    # find matching archives (base.TIMESTAMP.gz) sorted by mtime (newest first)
    mapfile -t archives < <(
      find "$dir" -maxdepth 1 -type f -name "${base}.*.gz" -printf '%T@ %p\n' 2>/dev/null \
      | sort -nr \
      | awk '{sub($1 FS, ""); print}'
    )
    if [ "${#archives[@]}" -gt "$KEEP" ]; then
      for ((i=KEEP;i<${#archives[@]};i++)); do
        rm -f -- "${archives[$i]}" || true
      done
    fi
  fi
done

exit 0
