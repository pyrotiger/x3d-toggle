#!/bin/bash
#X3D-Control v0.6.2_beta - install.sh
#Copyright (C) 2026 Pyrotiger

if [[ $EUID -ne 0 ]]; then
   echo "Error: This installer must be run with sudo."
   exit 1
fi

# Check for required dependencies
REQUIRED_CMDS="bc kdialog"
MISSING_DEPS=0

for cmd in $REQUIRED_CMDS; do
    if ! command -v $cmd &> /dev/null; then
        echo "Error: Required command '$cmd' not found."
        MISSING_DEPS=1
    fi
done

if [ $MISSING_DEPS -eq 1 ]; then
    echo "Please install missing dependencies and run the installer again."
    exit 1
fi

if [ -z "$SUDO_USER" ]; then
    echo "Error: This script must be run via sudo to correctly identify the user."
    exit 1
fi

SOURCE_DIR=$(dirname "$(readlink -f "$0")")
BIN_PATH="/usr/bin/x3d-control"
DAEMON_PATH="/usr/bin/x3d-daemon"
SERVICE_PATH="/usr/lib/systemd/user/x3d-auto.service"
ASSET_DIR="/usr/share/x3d-toggle"
SCRIPT_NAME="x3d-control"
DAEMON_NAME="x3d-daemon"
ICON_NAME="ryzen.jpeg"
SOURCE_ICON_PATH="assets/$ICON_NAME"
DESKTOP_FILE="/usr/share/applications/x3d-control.desktop"
HELPER_PATH="/usr/libexec/x3d-apply"
POLKIT_DIR="/usr/share/polkit-1/actions"
POLKIT_POLICY="org.x3dtoggle.policy"

mkdir -p "/usr/libexec"
mkdir -p "$POLKIT_DIR"
cp "$SOURCE_DIR/x3d-apply" "$HELPER_PATH"
chmod +x "$HELPER_PATH"
cp "$SOURCE_DIR/$POLKIT_POLICY" "$POLKIT_DIR/$POLKIT_POLICY"
chmod 644 "$POLKIT_DIR/$POLKIT_POLICY"

mkdir -p "$ASSET_DIR"
cp "$SOURCE_DIR/$SCRIPT_NAME" "$BIN_PATH"
cp "$SOURCE_DIR/$SOURCE_ICON_PATH" "$ASSET_DIR/$ICON_NAME"

chmod +x "$BIN_PATH"

echo "X3D Toggle Control: System Installation"
echo "-----------------"

if [ -f "$SOURCE_DIR/$DAEMON_NAME" ] && [ -f "$SOURCE_DIR/x3d-auto.service" ]; then
    cp "$SOURCE_DIR/$DAEMON_NAME" "$DAEMON_PATH"
    chmod +x "$DAEMON_PATH"

    mkdir -p /usr/lib/systemd/user/
    cp "$SOURCE_DIR/x3d-auto.service" "$SERVICE_PATH"
else
    echo "[-] Error: Daemon source files missing. Make sure x3d-daemon and x3d-auto.service are in the folder."
fi

echo "=/\= Autonomous Scheduler Setup..."
echo "The background automation daemon has been installed."
echo "Do you want to activate it now to dynamically toggle modes?"
echo " - YES: The daemon will start immediately."
echo " - NO:  The system will rely on default kernel CPPC scheduling."
read -p "Enable X3D Automation Daemon? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    if [ -n "$SUDO_USER" ]; then
        USER_UID=$(id -u "$SUDO_USER")
        sudo -u "$SUDO_USER" XDG_RUNTIME_DIR="/run/user/$USER_UID" systemctl --user enable --now x3d-auto
        echo "[+] Automation daemon actively running for $SUDO_USER."
    else
        echo "[!] Could not auto-start. Run manually: systemctl --user enable --now x3d-auto"
    fi
else
    echo "[-] Automation skipped. Default kernel CPPC scheduling remains active."
    echo "    To manually enable it later, run this command in your terminal(without sudo):"
    echo "    systemctl --user enable --now x3d-auto"
fi
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
Keywords=amd;x3d;vcache;gaming;llm;encode;streaming;compute;workload;
X-KDE-Keywords=x3d,vcache,cpu,rabbit,cheetah,llm,encode,streaming,compute,workload
EOF

    update-desktop-database /usr/share/applications/ 2>/dev/null
    echo "Launcher entry created: $DESKTOP_FILE"

    read -p "Create a shortcut on your physical Desktop? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        REAL_USER_HOME=$(eval echo "~$SUDO_USER")
      if [ -d "$REAL_USER_HOME/Desktop" ]; then
            cp "$DESKTOP_FILE" "$REAL_USER_HOME/Desktop/x3d-control.desktop"
            chmod +x "$REAL_USER_HOME/Desktop/x3d-control.desktop"
            chown $SUDO_USER:$SUDO_USER "$REAL_USER_HOME/Desktop/x3d-control.desktop"
            echo "Desktop shortcut linked for: $SUDO_USER"
        else
            echo "[-]Desktop directory not found. Skipping shortcut."
        fi
    fi
else
    echo "UI Integration skipped. Binary is ready: type 'x3d-control' in terminal."
fi

echo "--------------------------------------------------"
echo "Setup Complete.  Scotty, engage transwarp engines!"
echo "--------------------------------------------------"
echo "     =/\=      Live Long and Prosper     =/\=     "
