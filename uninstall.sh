#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "Error: This uninstaller must be run with sudo."
   exit 1
fi

BIN_PATH="/usr/bin/x3d-control"
ASSET_DIR="/usr/share/x3d-toggle"
DESKTOP_FILE="/usr/share/applications/x3d-control.desktop"

echo "Initiating removal of X3D Toggle Control..."

# 1. Eradicate binary
rm -f "$BIN_PATH"

# 2. Eradicate assets and directory
rm -rf "$ASSET_DIR"

# 3. Eradicate application launcher entry and update database
if [[ -f "$DESKTOP_FILE" ]]; then
    rm -f "$DESKTOP_FILE"
    update-desktop-database /usr/share/applications/ 2>/dev/null
    echo "Removed application launcher entry."
fi

# 4. Eradicate physical Desktop shortcut for the invoking user
if [[ -n "$SUDO_USER" ]]; then
    REAL_USER_HOME=$(eval echo "~$SUDO_USER")
    DESKTOP_LINK="$REAL_USER_HOME/Desktop/x3d-control.desktop"
    if [[ -L "$DESKTOP_LINK" || -f "$DESKTOP_LINK" ]]; then
        rm -f "$DESKTOP_LINK"
        echo "Removed desktop shortcut for: $SUDO_USER"
    fi
fi

echo "--------------------------------------------------"
echo "Uninstallation complete. Live long and prosper."
echo "--------------------------------------------------"
