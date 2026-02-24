# Maintainer: Pyrotiger
pkgname=x3d-toggle-git
pkgver=0.5.0-beta
pkgrel=1
pkgdesc="AMD 3D V-Cache Technology Toggle Control - Community Edition"
arch=('any')
url="https://github.com/pyrotiger/x3d-toggle"
license=('GPL3')
depends=('kdialog' 'polkit' 'libnotify')
makedepends=('git')
provides=("${pkgname%-git}")
conflicts=("${pkgname%-git}")
source=("git+https://github.com/pyrotiger/x3d-toggle.git")
md5sums=('SKIP')

package() {
    cd "$srcdir/${pkgname%-git}"
    
    # Define package deployment paths
    _bindir="$pkgdir/usr/bin"
    _sharedir="$pkgdir/usr/share/x3d-toggle"
    _appdir="$pkgdir/usr/share/applications"

    # Create directory structures
    install -dm755 "$_bindir" "$_sharedir" "$_appdir"

    # Deploy primary executable
    install -Dm755 x3d_control "$_bindir/x3d-control"
    
    # Deploy notification asset from the assets directory
    install -Dm644 assets/ryzen.jpeg "$_sharedir/ryzen.jpeg"

    # Generate the Desktop Entry automatically
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
Keywords=amd;x3d;vcache;gaming;llm;
X-KDE-Keywords=x3d,vcache,cpu,rabbit,cheetah,llm
EOF
    chmod 644 "$_appdir/x3d-control.desktop"
}
