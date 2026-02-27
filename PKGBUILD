# shellcheck shell=bash
# shellcheck disable=SC2034,SC2154,SC2164
# Maintainer: Pyrotiger

_pkgname=x3d-toggle
pkgname="${_pkgname}-git"
pkgver=v1.0.0
pkgrel=1
pkgdesc="AMD 3D V-Cache Technology Toggle Control - Community Edition"
arch=('x86_64')
url="https://github.com/pyrotiger/x3d-toggle"
license=('GPL-3.0-only')
depends=('kdialog' 'polkit' 'libnotify' 'procps-ng')
optdepends=('gamemode: For GameMode integration'
            'steam: For Steam game detection' 
            'wine: For Windows game compatibility' 
            'proton: For Steam Play compatibility')
makedepends=('git')
options=('!debug')
provides=("${_pkgname}")
conflicts=("${_pkgname}")
source=("git+${url}.git")
md5sums=('SKIP')
backup=('etc/x3d-toggle.conf')
install='x3d-toggle.install'

pkgver() {
    local version revision commit

    cd "$srcdir/${_pkgname}" || return 1

    version="$(sed -n 's/^#define VERSION "\([^"]*\)"$/\1/p' x3d-toggle.c | head -n1)"
    [ -n "$version" ] || version="0.0.0"

    revision="$(git rev-list --count HEAD)"
    commit="$(git rev-parse --short HEAD)"
    printf 'v%s.r%s.g%s' "$version" "$revision" "$commit"
}

prepare() {
    cd "$srcdir/${_pkgname}" || return 1
    cat <<EOF > "x3d-toggle-gui.desktop"
[Desktop Entry]
Type=Application
Name=X3D CCD Toggle
GenericName=AMD 3D v-Cache Technology Mode Switcher
Comment=Toggle between Rabbit (Cache) and Cheetah (Frequency) modes
Exec=/usr/bin/x3d-toggle-gui
Icon=/usr/share/x3d-toggle/ryzen.jpeg
Terminal=false
Categories=System;Settings;
Keywords=amd;x3d;vcache;gaming;llm;encode;streaming;compute;workload;
X-KDE-Keywords=x3d,vcache,cpu,rabbit,cheetah,llm,encode,streaming,compute,workload
EOF
}

build() {
    cd "$srcdir/${_pkgname}" || return 1
    make PREFIX=/usr CPPFLAGS="$CPPFLAGS" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"
}

package() {
    cd "$srcdir/${_pkgname}" || return 1

    install -Dm755 "x3d-toggle-c" "$pkgdir/usr/bin/x3d-toggle-c"
    install -Dm755 "x3d-toggle-gui" "$pkgdir/usr/bin/x3d-toggle-gui"
    install -Dm755 "x3d-daemon" "$pkgdir/usr/bin/x3d-daemon"
    install -Dm644 "assets/ryzen.jpeg" "$pkgdir/usr/share/x3d-toggle/ryzen.jpeg"
    install -Dm644 "org.x3dtoggle.policy" "$pkgdir/usr/share/polkit-1/actions/org.x3dtoggle.policy"
    install -Dm644 "x3d-toggle.conf" "$pkgdir/etc/x3d-toggle.conf"
    install -Dm644 "x3d-auto.service" "$pkgdir/usr/lib/systemd/user/x3d-auto.service"
    install -Dm644 "LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
    install -Dm644 "x3d-toggle-gui.desktop" "$pkgdir/usr/share/applications/x3d-toggle-gui.desktop"
}
