#Maintainer: Pyrotiger
#X3D-Control v0.6.2_beta - PKGBUILD
#Copyright (C) 2026 Pyrotiger

pkgname=x3d-toggle-git
pkgver=0.6.2_beta
pkgrel=1
pkgdesc="AMD 3D V-Cache Technology Toggle Control - Community Edition"
arch=('any')
url="https://github.com/pyrotiger/x3d-toggle"
license=('GPL3')
depends=('kdialog' 'polkit' 'libnotify' 'bc' 'procps-ng')
makedepends=('git')
provides=("${pkgname%-git}")
conflicts=("${pkgname%-git}")
source=("git+https://github.com/pyrotiger/x3d-toggle.git")
md5sums=('SKIP')
##pkgname=x3d-toggle
##pkgver=1.0.0
##source=("https://github.com/pyrotiger/x3d-toggle/archive/refs/tags/v${pkgver}.tar.gz")
##sha256sums=('xxx') # xxx replace with actual values

package() {
    cd "$srcdir/${pkgname%-git}"
    
    _bindir="$pkgdir/usr/bin"
    _libexecdir="$pkgdir/usr/libexec"
    _sharedir="$pkgdir/usr/share/x3d-toggle"
    _polkitdir="$pkgdir/usr/share/polkit-1/actions"
    _appdir="$pkgdir/usr/share/applications"
    _servicedir="$pkgdir/usr/lib/systemd/user"

    install -dm755 "$_bindir" "$_libexecdir" "$_sharedir" "$_appdir" "$_servicedir"
    install -Dm755 x3d-control "$_bindir/x3d-control"
    install -Dm755 x3d-daemon "$_bindir/x3d-daemon"
    install -Dm755 x3d-apply "$_libexecdir/x3d-apply"
    install -Dm644 org.x3dtoggle.policy "$_polkitdir/org.x3dtoggle.policy"
    install -Dm644 x3d-auto.service "$_servicedir/x3d-auto.service"
    install -Dm644 assets/ryzen.jpeg "$_sharedir/ryzen.jpeg"
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/${pkgname%-git}/LICENSE"

    cat <<EOF > "$_appdir/x3d-control.desktop"
[Desktop Entry]
Type=Application
Name=X3D CCD Toggle Control
GenericName=AMD 3D v-Cache Technology Mode Switcher
Comment=Toggle between Rabbit (Cache) and Cheetah (Frequency) modes
Exec=/usr/bin/x3d-control
Icon=/usr/share/x3d-toggle/ryzen.jpeg
Terminal=false
Categories=System;Settings;
Keywords=amd;x3d;vcache;gaming;llm;encode;streaming;compute;workload;
X-KDE-Keywords=x3d,vcache,cpu,rabbit,cheetah,llm,encode,streaming,compute,workload
EOF
    chmod 644 "$_appdir/x3d-control.desktop"
}
