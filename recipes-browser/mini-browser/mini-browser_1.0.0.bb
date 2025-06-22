SUMMARY = "Mini Browser GTK3 WebKit Application"
DESCRIPTION = "A minimal fullscreen browser using GTK3 and WebKit"
LICENSE = "CLOSED"

SRC_URI = "file://mini-browser.c"

S = "${WORKDIR}"

# Dependencies for GTK3 and WebKit
DEPENDS = "gtk+3 webkitgtk"

# Runtime dependencies
RDEPENDS:${PN} = "gtk+3 webkitgtk"

# Inherit pkgconfig for pkg-config support
inherit pkgconfig

do_compile() {
    # Compile the application using pkg-config for GTK3 and WebKit
    ${CC} ${CFLAGS} ${LDFLAGS} \
        `pkg-config --cflags gtk+-3.0` \
        `pkg-config --cflags webkit2gtk-4.0` \
        ${S}/mini-browser.c \
        `pkg-config --libs gtk+-3.0` \
        `pkg-config --libs webkit2gtk-4.0` \
        -o mini-browser
}

do_install() {
    # Create binary directory
    install -d ${D}${bindir}
    
    # Install the binary
    install -m 0755 ${S}/mini-browser ${D}${bindir}/
}

FILES:${PN} = "${bindir}/mini-browser"