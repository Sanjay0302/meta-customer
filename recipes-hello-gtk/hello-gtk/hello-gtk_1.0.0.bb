DESCRIPTION = "Simple GTK 3.0 Hello World Application"
LICENSE = "CLOSED"
SRC_URI = "file://hello_gtk.c"

S = "${WORKDIR}"

DEPENDS = "gtk+3"

inherit pkgconfig

do_compile() {
  ${CC} ${LDFLAGS} hello_gtk.c -o hello_gtk `pkg-config --cflags --libs gtk+-3.0`
}

do_install() {
  install -d ${D}${bindir}
  install -m 0755 hello_gtk ${D}${bindir}
}

FILES_${PN} += "${bindir}/hello_gtk"
