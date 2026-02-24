#!/bin/bash

# 1. Root Privilege Check
if [[ $EUID -ne 0 ]]; then
   echo "Error: This uninstaller must be run with sudo."
   exit 1
fi

BIN_PATH="/usr/bin/x3d-control"
ASSET_DIR="/usr/share/x3d-toggle"
DESKTOP_FILE="/usr/share/applications/x3d-control.desktop"

echo "X3D Toggle Control: System Removal"
echo "----------------------------------"

# 2. Binary and Asset Purge
if [ -f "$BIN_PATH" ]; then
    rm "$BIN_PATH"
    echo "Removed: $BIN_PATH"
fi

if [ -d "$ASSET_DIR" ]; then
    rm -rf "$ASSET_DIR"
    echo "Removed: $ASSET_DIR"
fi

# 3. UI Integration Cleanup
if [ -f "$DESKTOP_FILE" ]; then
    rm "$DESKTOP_FILE"
    echo "Removed: $DESKTOP_FILE"
    update-desktop-database /usr/share/applications/ 2>/dev/null
fi

# 4. User-Specific Desktop Link Cleanup
if [ -n "$SUDO_USER" ]; then
    REAL_USER_HOME=$(eval echo "~$SUDO_USER")
    USER_DESKTOP_LINK="$REAL_USER_HOME/Desktop/x3d-control.desktop"

    if [ -L "$USER_DESKTOP_LINK" ] || [ -f "$USER_DESKTOP_LINK" ]; then
        rm "$USER_DESKTOP_LINK"
        echo "Removed desktop shortcut for: $SUDO_USER"
    fi
fi

echo "--------------------------------------------------"
echo "Cleanup Complete."
echo "--------------------------------------------------"
