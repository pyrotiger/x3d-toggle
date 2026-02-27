#Maintainer: Pyrotiger
#X3D-Control v0.6.4_beta - PKGBUILD
#Copyright (C) 2026 Pyrotiger

pkgname=x3d-toggle-git
pkgver=v0.6.4_beta.r8.g4961177
pkgrel=1
pkgdesc="AMD 3D V-Cache Technology Toggle Control - Community Edition"
arch=('any')
url="https://github.com/pyrotiger/x3d-toggle"
license=('GPL3')
depends=('kdialog' 'polkit' 'libnotify' 'procps-ng')
optdepends=('gamemode: For GameMode integration' 
            'steam: For Steam game detection' 
            'wine: For Windows game compatibility' 
            'proton: For Steam Play compatibility')
makedepends=('git')
provides=("${pkgname%-git}")
conflicts=("${pkgname%-git}")
source=("git+file://${srcdir}/../../")
md5sums=('SKIP')
##pkgname=x3d-toggle
##pkgver=1.0.0
##source=("https://github.com/pyrotiger/x3d-toggle/archive/refs/tags/v${pkgver}.tar.gz")
##sha256sums=('xxx') # xxx replace with actual values

backup=('etc/x3d-toggle.conf')
install='x3d-toggle.install'

pkgver() {
    cd "$srcdir/${pkgname%-git}"
    printf "v%s.r%s.g%s" "$(grep -Po 'VERSION "\K[^"]*' x3d-toggle.c)" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
    cat <<EOF > "x3d-control.desktop"
[Desktop Entry]
Type=Application
Name=X3D CCD Toggle
GenericName=AMD 3D v-Cache Technology Mode Switcher
Comment=Toggle between Rabbit (Cache) and Cheetah (Frequency) modes
Exec=/usr/bin/x3d-control
Icon=/usr/share/x3d-toggle/ryzen.jpeg
Terminal=false
Categories=System;Settings;
Keywords=amd;x3d;vcache;gaming;llm;encode;streaming;compute;workload;
X-KDE-Keywords=x3d,vcache,cpu,rabbit,cheetah,llm,encode,streaming,compute,workload
EOF
}

build() {
    cd "$srcdir/${pkgname%-git}"
    make PREFIX=/usr
}

package() {
    cd "$srcdir/${pkgname%-git}"
    
    install -Dm755 "x3d-toggle-c" "$pkgdir/usr/bin/x3d-toggle-c"
    install -Dm755 "x3d-control" "$pkgdir/usr/bin/x3d-control"
    install -Dm755 "x3d-daemon" "$pkgdir/usr/bin/x3d-daemon"
    install -Dm644 "assets/ryzen.jpeg" "$pkgdir/usr/share/x3d-toggle/ryzen.jpeg"
    install -Dm644 "org.x3dtoggle.policy" "$pkgdir/usr/share/polkit-1/actions/org.x3dtoggle.policy"
    install -Dm644 "x3d-toggle.conf" "$pkgdir/etc/x3d-toggle.conf"
    install -Dm644 "x3d-auto.service" "$pkgdir/usr/lib/systemd/user/x3d-auto.service"
    install -Dm644 "LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
    install -Dm644 "$srcdir/x3d-control.desktop" "$pkgdir/usr/share/applications/x3d-control.desktop"
}
