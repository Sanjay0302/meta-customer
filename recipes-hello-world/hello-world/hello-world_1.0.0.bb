# Package summary
SUMMARY = "Hello World"
# License, for example MIT
LICENSE = "MIT"
# License checksum file is always required
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

# hello-world.c from local file
SRC_URI = "file://hello-world.c"

# Set LDFLAGS options provided by the build system
TARGET_CC_ARCH += "${LDFLAGS}"

# Change source directory to workdirectory where hello-world.cpp is
S = "${WORKDIR}"

# Compile hello-world from sources, no Makefile
do_compile() {
    ${CXX} -Wall hello-world.c -o hello-world
}

# Install binary to final directory /usr/bin
do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/hello-world ${D}${bindir}
}
