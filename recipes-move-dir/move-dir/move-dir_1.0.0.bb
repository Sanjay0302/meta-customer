SUMMARY = "Move assets directory to home"
DESCRIPTION = "Recipe to install assets directory with index.html to /home"
LICENSE = "CLOSED"

SRC_URI = "file://assets"

S = "${WORKDIR}"

do_install() {
    # Create the destination directory
    install -d ${D}/home
    
    # Copy the assets directory with all contents
    cp -r ${S}/assets ${D}/home/
    
    # Set proper permissions
    chmod -R 644 ${D}/home/assets/*
    find ${D}/home/assets -type d -exec chmod 755 {} \;
}

FILES:${PN} = "/home/assets/*"

# Optional: Set ownership to root
pkg_postinst:${PN}() {
    if [ -n "$D" ]; then
        # During build
        chown -R root:root $D/home/assets
    else
        # On target
        chown -R root:root /home/assets
    fi
}