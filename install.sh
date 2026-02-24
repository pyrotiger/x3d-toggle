#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "Error: This installer must be run with sudo."
   exit 1
fi

SOURCE_DIR=$(dirname "$(readlink -f "$0")")
BIN_PATH="/usr/bin/x3d-control"
ASSET_DIR="/usr/share/x3d-toggle"
SCRIPT_NAME="x3d_control.sh"
ICON_NAME="ryzen.jpeg"
SOURCE_ICON_PATH="assets/$ICON_NAME"
DESKTOP_FILE="/usr/share/applications/x3d-control.desktop"

mkdir -p "$ASSET_DIR"
cp "$SOURCE_DIR/$SCRIPT_NAME" "$BIN_PATH"
cp "$SOURCE_DIR/$SOURCE_ICON_PATH" "$ASSET_DIR/$ICON_NAME"

chmod +x "$BIN_PATH"

echo "X3D Toggle Control: System Installation"
echo "-----------------"

read -p "Create Application Launcher entry? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    cat <<EOF > "$DESKTOP_FILE"
[Desktop Entry]
Type=Application
Name=X3D CCD Toggle
GenericName=AMD 3D v-Cache Technology Mode Switcher
Comment=Toggle between Rabbit (Cache) and Cheetah (Frequency) modes
Exec=$BIN_PATH
Icon=$ASSET_DIR/$ICON_NAME
Terminal=false
Categories=System;Settings;
Keywords=amd;x3d;vcache;gaming;llm;
X-KDE-Keywords=x3d,vcache,cpu,rabbit,cheetah,llm
EOF

    update-desktop-database /usr/share/applications/ 2>/dev/null
    echo "Launcher entry created: $DESKTOP_FILE"

    read -p "Create a shortcut on your physical Desktop? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        REAL_USER_HOME=$(eval echo "~$SUDO_USER")
        ln -sf "$DESKTOP_FILE" "$REAL_USER_HOME/Desktop/"
        chown $SUDO_USER:$SUDO_USER "$REAL_USER_HOME/Desktop/x3d-control.desktop"
        echo "Desktop shortcut linked for: $SUDO_USER"
    fi
else
    echo "UI Integration skipped. Binary is ready: 'x3d-control' in terminal."
fi

echo "--------------------------------------------------"
echo "Setup Complete.  Scotty, engage transwarp engines!"
echo "--------------------------------------------------"
