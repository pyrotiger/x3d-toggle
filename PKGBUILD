# Maintainer: Pyrotiger
pkgname=amd-x3d-toggle
pkgver=0.5.0-beta
pkgrel=1
pkgdesc="GUI utility to toggle CCD priority on AMD Ryzen 9 X3D CPUs (9950X3D)"
arch=('x86_64')
url="https://github.com/your-repo/amd-x3d-toggle"
license=('GPL')
depends=('bash' 'kdialog' 'polkit' 'libnotify')
source=("x3d-control.sh"
        "ryzen.jpeg")
sha256sums=('SKIP' 'SKIP')

package() {
    # 1. Install Executable Binary
    install -Dm755 "${srcdir}/x3d-control.sh" "${pkgdir}/usr/bin/x3d-control"

    # 2. Install Assets to /usr/share
    install -Dm644 "${srcdir}/assets/ryzen.jpeg" "${pkgdir}/usr/share/x3d-toggle/ryzen.jpeg"

    # 3. Create and Install Desktop Entry
    mkdir -p "${pkgdir}/usr/share/applications/"
    cat <<EOF > "${pkgdir}/usr/share/applications/x3d-control.desktop"
[Desktop Entry]
Type=Application
Name=X3D CCD Toggle Control
GenericName=AMD X3D Mode Switcher
Comment=Toggle between Rabbit (Cache) and Cheetah (Frequency) modes
Exec=/usr/bin/x3d-control
Icon=/usr/share/x3d-toggle/ryzen.jpeg
Terminal=false
Categories=System;Settings;
Keywords=amd;x3d;vcache;gaming;llm;
X-KDE-Keywords=x3d,vcache,cpu,rabbit,cheetah,llm
EOF
}
