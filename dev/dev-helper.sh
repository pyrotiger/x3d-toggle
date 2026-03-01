#!/usr/bin/env bash
# Consolidated dev helper for installing/removing dev policy and binary
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LOG_DIR="$REPO_ROOT/logs"
mkdir -p "$LOG_DIR"
DEV_BIN_SRC="$REPO_ROOT/x3d-toggle"
DEV_BIN_DST="/usr/bin/x3d-toggle.dev"
POLICY_SRC="$REPO_ROOT/dev/org.x3dtoggle.policy.dev"
POLICY_DST="/usr/share/polkit-1/actions/org.x3dtoggle.dev.policy"
RULES_SRC="$REPO_ROOT/dev/49-org.x3dtoggle.rules.local"
RULES_DST="/etc/polkit-1/rules.d/49-org.x3dtoggle.rules"

# By default run dev-mode in sandboxed, non-privileged directories under dev/sandbox.
# Pass the positional command `--unsafe` before action to perform real system installs.
SANDBOX_ROOT="$REPO_ROOT/dev/sandbox"
SANDBOX=true

maybe_run_real() {
  # Parse leading --unsafe and any --purge flag anywhere
  # SANDBOX_ARG_CONSUMED is unused intentionally; only parse flags into ARGS
  PURGE=false
  ARGS=()
  for a in "$@"; do
    case "$a" in
      --unsafe)
        SANDBOX=false
        echo "WARNING: running in UNSAFE mode; operations may touch system paths" >> "$LOG_DIR/dev-helper.log"
        ;;
      --purge)
        PURGE=true
        ;;
      *)
        ARGS+=("$a")
        ;;
    esac
  done
}

usage() {
  cat <<EOF
Usage: $(basename "$0") <command>

Commands:
  install-all    Install dev binary, dev policy, and local .rules (if present)
  install-bin    Install only the dev binary and dev action
  install-rules   Install only the local .rules (requires file at dev/49-org.x3dtoggle.rules.local)
  remove-all     Remove installed dev binary, dev policy and local .rules
  status         Print what would be/are installed
  test           Run a privileged invocation test (pkexec) to exercise policy
  help           Show this help
EOF
}

ensure_group() {
  if ! getent group x3dtoggle >/dev/null; then
    echo "Creating system group 'x3dtoggle'..."
    sudo groupadd --system x3dtoggle
  fi
}

install_bin() {
  if [ ! -x "$DEV_BIN_SRC" ]; then
    echo "Error: built binary not found at $DEV_BIN_SRC" >&2
    return 1
  fi
  echo "Installing dev binary and policy (sandbox=$SANDBOX)"
  echo "$(date --iso-8601=seconds) install_bin: installing binary and policy (sandbox=$SANDBOX)" >> "$LOG_DIR/dev-helper.log"
  if [ "$SANDBOX" = true ]; then
    mkdir -p "$SANDBOX_ROOT/usr/bin" "$SANDBOX_ROOT/usr/share/polkit-1/actions"
    install -Dm755 "$DEV_BIN_SRC" "$SANDBOX_ROOT/usr/bin/x3d-toggle.dev"
    install -Dm644 "$POLICY_SRC" "$SANDBOX_ROOT/usr/share/polkit-1/actions/org.x3dtoggle.dev.policy"
    echo "Sandboxed dev binary installed under $SANDBOX_ROOT/usr/bin/x3d-toggle.dev"
  else
    sudo install -Dm755 "$DEV_BIN_SRC" "$DEV_BIN_DST"
    sudo install -Dm644 "$POLICY_SRC" "$POLICY_DST"
  fi
}

install_rules() {
  if [ ! -f "$RULES_SRC" ]; then
    echo "RULES source not found at $RULES_SRC" >&2
    return 1
  fi
  ensure_group
  echo "Adding user '$USER' to group 'x3dtoggle' (may require re-login)..."
  echo "$(date --iso-8601=seconds) install_rules: installing .rules" >> "$LOG_DIR/dev-helper.log"
  if [ "$SANDBOX" = true ]; then
    mkdir -p "$SANDBOX_ROOT/etc/polkit-1/rules.d"
    install -Dm644 "$RULES_SRC" "$SANDBOX_ROOT/etc/polkit-1/rules.d/49-org.x3dtoggle.rules"
    echo "Sandboxed .rules installed under $SANDBOX_ROOT/etc/polkit-1/rules.d/"
  else
    sudo usermod -aG x3dtoggle "$USER" || true
    echo "Installing polkit JS .rules to $RULES_DST"
    sudo install -Dm644 "$RULES_SRC" "$RULES_DST"
  fi
}

reload_polkit() {
  echo "Reloading polkit (if supported)"
  if command -v systemctl >/dev/null 2>&1; then
    sudo systemctl reload polkit.service 2>/dev/null || sudo systemctl restart polkit.service 2>/dev/null || true
  fi
}

remove_all() {
  echo "Removing $DEV_BIN_DST and $POLICY_DST and local .rules"
  echo "$(date --iso-8601=seconds) remove_all: removing installed artifacts (sandbox=$SANDBOX)" >> "$LOG_DIR/dev-helper.log"
  if [ "$SANDBOX" = true ]; then
    if [ -d "$SANDBOX_ROOT" ]; then
      echo "Attempting to unmount any mounts under $SANDBOX_ROOT"
      # Try to lazily unmount anything mounted beneath the sandbox
      if command -v findmnt >/dev/null 2>&1; then
        mapfile -t MOUNTS < <(findmnt -rn -o TARGET --target "$SANDBOX_ROOT" || true)
        for m in "${MOUNTS[@]}"; do
          if [ -n "$m" ]; then
            umount -l "$m" 2>/dev/null || true
          fi
        done
      fi

      rm -rf "$SANDBOX_ROOT"
      sync || true
      if [ -d "$SANDBOX_ROOT" ]; then
        echo "Warning: failed to fully remove $SANDBOX_ROOT; manual cleanup may be required." >&2
      else
        echo "Removed sandbox at $SANDBOX_ROOT"
      fi
    else
      echo "No sandbox present at $SANDBOX_ROOT"
    fi

    if [ "$PURGE" = true ]; then
      echo "Purging dev logs as requested"
      rm -f "$LOG_DIR/dev-helper.log" || true
    fi
  else
    sudo rm -f "$DEV_BIN_DST" "$POLICY_DST"
    sudo rm -f "$RULES_DST"
    reload_polkit
  fi
}

status() {
  echo "Dev binary:    $( [ -x "$DEV_BIN_DST" ] && echo 'installed' || echo 'missing' )"
  echo "Dev policy:    $( [ -f "$POLICY_DST" ] && echo 'installed' || echo 'missing' )"
  echo "Local .rules:   $( [ -f "$RULES_DST" ] && echo 'installed' || echo 'missing (copy dev/49-org.x3dtoggle.rules.local to enable)' )"
}

test_pkexec() {
  if [ ! -x "$DEV_BIN_DST" ]; then
    if [ "$SANDBOX" = true ]; then
      if [ ! -x "$SANDBOX_ROOT/usr/bin/x3d-toggle.dev" ]; then
        echo "Sandboxed dev binary not installed. Run 'install-bin' first." >&2
        return 1
      fi
    else
      echo "Dev binary not installed at $DEV_BIN_DST" >&2
      return 1
    fi
  fi
  if [ "$SANDBOX" = true ]; then
    echo "Running sandboxed dev binary (no privileged pkexec)"
    # Prefer bubblewrap if available for an extra sandbox layer
    if command -v bwrap >/dev/null 2>&1; then
      bwrap --ro-bind /usr /usr --ro-bind /lib /lib --ro-bind /lib64 /lib64 \
            --ro-bind /etc /etc --ro-bind /bin /bin --ro-bind /sbin /sbin \
            --bind "$SANDBOX_ROOT" "$SANDBOX_ROOT" \
            --dev /dev --proc /proc --tmpfs /tmp --unshare-all --die-with-parent "$SANDBOX_ROOT/usr/bin/x3d-toggle.dev" get
    else
      "$SANDBOX_ROOT/usr/bin/x3d-toggle.dev" get
    fi
  else
    echo "This will prompt for auth (or be passwordless if .rules installed)."
    echo "Running: pkexec $DEV_BIN_DST get"
    pkexec "$DEV_BIN_DST" get
  fi
}

maybe_run_real "$@"
case ${ARGS[0]:-help} in
  install-all)
    install_bin
    if [ -f "$RULES_SRC" ]; then
      install_rules
    else
      echo "Note: $RULES_SRC not found — local .rules not installed. Create dev/49-org.x3dtoggle.rules.local from the example to enable passwordless dev UX."
    fi
    if [ "$SANDBOX" = false ]; then
      reload_polkit
    fi
    ;;
  install-bin)
    install_bin
    if [ "$SANDBOX" = false ]; then reload_polkit; fi
    ;;
  install-rules)
    install_rules
    if [ "$SANDBOX" = false ]; then reload_polkit; fi
    ;;
  remove-all)
    remove_all
    ;;
  status)
    status
    ;;
  test)
    test_pkexec
    ;;
  help|--help|-h)
    usage
    ;;
  *)
    echo "Unknown command: $1" >&2
    usage
    exit 2
    ;;
esac
