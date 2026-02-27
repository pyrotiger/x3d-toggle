# shellcheck shell=bash
# shellcheck disable=SC2034,SC2154,SC2164
# Maintainer: Pyrotiger

_pkgname=x3d-toggle
pkgname="${_pkgname}"
pkgver=1.0.2
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
# Removed makedepends on git as we use a release tarball now
options=('!debug')
provides=("${_pkgname}")
conflicts=("${_pkgname}" "${_pkgname}-git")
source=("${pkgname}-${pkgver}.tar.gz::${url}/archive/refs/tags/v${pkgver}.tar.gz")
sha256sums=('5340692f12cec5fce5dc46a5605514e9a82b1f39dd64ef2a2c0913e16ebe2a21')
backup=('etc/x3d-toggle.conf')
install='x3d-toggle.install'

# Removed pkgver() since we use fixed versions for release packages

prepare() {
    cd "$srcdir/${_pkgname}-${pkgver}" || return 1
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
    cd "$srcdir/${_pkgname}-${pkgver}" || return 1
    make PREFIX=/usr CPPFLAGS="$CPPFLAGS" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"
}

package() {
    cd "$srcdir/${_pkgname}-${pkgver}" || return 1

    # Utilize Makefile for logically sound installation
    make DESTDIR="$pkgdir" PREFIX=/usr install
    install -Dm644 "x3d-toggle-gui.desktop" "$pkgdir/usr/share/applications/x3d-toggle-gui.desktop"
}
