#!/bin/bash
#X3D-Control v0.6.4_beta - uninstall.sh
#Copyright (C) 2026 Pyrotiger

if [[ $EUID -ne 0 ]]; then
   echo "Error: This uninstaller must be run with sudo."
   exit 1
fi

BIN_PATH="/usr/bin/x3d-control"
ASSET_DIR="/usr/share/x3d-toggle"
DESKTOP_FILE="/usr/share/applications/x3d-control.desktop"

echo "Initiating removal of X3D Toggle Control..."

if [[ -n "$SUDO_USER" ]]; then
    USER_UID=$(id -u "$SUDO_USER")
    # Stop the running service
    sudo -u "$SUDO_USER" XDG_RUNTIME_DIR="/run/user/$USER_UID" systemctl --user stop x3d-auto 2>/dev/null
    # Disable it from starting on boot
    sudo -u "$SUDO_USER" XDG_RUNTIME_DIR="/run/user/$USER_UID" systemctl --user disable x3d-auto 2>/dev/null
    echo "Stopped and disabled background automation."
fi

rm -f "$BIN_PATH"
rm -f "/usr/bin/x3d-daemon"
rm -f "/usr/lib/systemd/user/x3d-auto.service"
rm -f "/usr/libexec/x3d-apply"
rm -f "/usr/share/polkit-1/actions/org.x3dtoggle.policy"
rm -f "/etc/x3d-toggle.conf"
rm -rf "$ASSET_DIR"

if [[ -f "$DESKTOP_FILE" ]]; then
    rm -f "$DESKTOP_FILE"
    update-desktop-database /usr/share/applications/ 2>/dev/null
    echo "Removed application launcher entry."
fi

if [[ -n "$SUDO_USER" ]]; then
    REAL_USER_HOME=$(eval echo "~$SUDO_USER")
    DESKTOP_LINK="$REAL_USER_HOME/Desktop/x3d-control.desktop"
    if [[ -L "$DESKTOP_LINK" || -f "$DESKTOP_LINK" ]]; then
        rm -f "$DESKTOP_LINK"
        echo "Removed desktop shortcut for: $SUDO_USER"
    fi
fi

echo "--------------------------------------------------"
echo "Setup Decomissioned. Scotty, you're retired!      "
echo "--------------------------------------------------"
echo "     =/\=      Live Long and Prosper     =/\=     "
