# Recipes-browser (tls-ignored)

This `recipes-mini-browser` has a mini-browser made from gtk+3 and webkit2gtk

The RDEPENDS (run time dependencies) are webkitgtk and gtk+3

Follow the below recipe structure

```bash
recipes-browser
└── mini-browser
    ├── CHANGELOG
    ├── files
    │   └── mini-browser.c
    ├── mini-browser_1.0.0.bb
    └── README - (You are currently reading me)
```

The default URL is `https://google.com`, modify the load_uri api in the `mini-browser.c`

add these lines to `path/to/poky/build/conf/local.conf`

```bash
IMAGE_INSTALL:append = " mini-browser webkitgtk gtk+3"
```

---

## Issues

When we run this application from terminal of the `core-image-sato`, we get the following error

```bash
libEGL warbibg: DRI2: failed to authenticate
Could not create EGL sharing context
```

This happens in embedded linux processors

The `mini-browser.c` is is optimised by turning of the Hardware accelaration feature.

Solution 2: Turn off the Hardware accelation feature separately.

If the issue issue persist with this approach, use this below cmdline environment argument before running the browser application.

```bash
export WEBKIT_DISABLE_COMPOSITING_MODE=1
export WEBKIT_DISABLE_DMABUF_RENDERER=1
export GDK_RENDERING=image
export GSK_RENDERER=cairo
./mini-browser
```

This issue will be resolved in next patch, the possible solution are listed below.

Solution 3: Adding lines in a wrapper shell script

This can be automatically done by creating the wrapper for executing the application

Modify the `.bb` file as follow

```bb
SUMMARY = "Mini Browser GTK4 WebKit Application"
DESCRIPTION = "A minimal fullscreen browser using GTK4 and WebKit"
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
    
    # Create a wrapper script with environment variables
    install -d ${D}${bindir}
    cat > ${D}${bindir}/mini-browser-wrapper << 'EOF'
#!/bin/bash
export WEBKIT_DISABLE_COMPOSITING_MODE=1
export WEBKIT_DISABLE_DMABUF_RENDERER=1
export GDK_RENDERING=image
export GSK_RENDERER=cairo
exec /usr/bin/mini-browser "$@"
EOF
    chmod +x ${D}${bindir}/mini-browser-wrapper
}

FILES:${PN} = "${bindir}/mini-browser ${bindir}/mini-browser-wrapper"
```

> Solution 4 for the issue is, install the hardware accelaration dependency
>
> ```bb
> IMAGE_INSTALL += "mini-browser gtk+3 webkitgtk mesa libdrm"
> ```

---


# Error in finding webkitgtk in pkg-config in st-image-weston

The .bb file uses webkitgtk-4.0, in st-image-weston the available package is webkitgtk-4.1, so use the below bb file to first check the available wenkitgtk version, then use the available pkg-config webkitgtk version in .bb

```bb
SUMMARY = "Mini Browser GTK3 WebKit2 Application with IGNORE TLS CA"
DESCRIPTION = "A minimal fullscreen browser using GTK3 and WebKit2"
LICENSE = "CLOSED"

SRC_URI = "file://mini-browser.c"

S = "${WORKDIR}"

# Try webkitgtk3 first
DEPENDS = "gtk+3 webkitgtk3"
RDEPENDS:${PN} = "gtk+3 webkitgtk3"

inherit pkgconfig

do_compile() {
    # Debug: Check what webkit pkg-config files are available
    echo "=== DEBUG: Available webkit pkg-config files ==="
    pkg-config --list-all | grep webkit || echo "No webkit packages found in pkg-config"
    
    echo "=== DEBUG: All pkg-config files ==="
    pkg-config --list-all | head -20
    
    # Common webkit pkg-config names to test
    for pkg in webkit2gtk-4.0 webkit2gtk-3.0 webkitgtk-3.0 webkit2gtk webkitgtk; do
        if pkg-config --exists $pkg; then
            echo "Found: $pkg"
            WEBKIT_PKG=$pkg
            break
        else
            echo "Not found: $pkg"
        fi
    done
    
    if [ -z "$WEBKIT_PKG" ]; then
        echo "ERROR: No webkit pkg-config package found"
        exit 1
    fi
    
    echo "Using webkit package: $WEBKIT_PKG"
    
    # Compile the application
    ${CC} ${CFLAGS} ${LDFLAGS} \
        `pkg-config --cflags gtk+-3.0` \
        `pkg-config --cflags ${WEBKIT_PKG}` \
        ${S}/mini-browser.c \
        `pkg-config --libs gtk+-3.0` \
        `pkg-config --libs ${WEBKIT_PKG}` \
        -o mini-browser
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/mini-browser ${D}${bindir}/
}

FILES:${PN} = "${bindir}/mini-browser"
```


So in my case it was webkitgtk-4.1, so for that here is the changed .bb file

```bb
SUMMARY = "Mini Browser GTK3 WebKit2 Application with IGNORE TLS CA"
DESCRIPTION = "A minimal fullscreen browser using GTK3 and WebKit2"
LICENSE = "CLOSED"

SRC_URI = "file://mini-browser.c"

S = "${WORKDIR}"

# Use webkitgtk3 (which provides webkit2gtk-4.1)
DEPENDS = "gtk+3 webkitgtk3"
RDEPENDS:${PN} = "gtk+3 webkitgtk3"

inherit pkgconfig

do_compile() {
    # Compile the application using webkit2gtk-4.1
    ${CC} ${CFLAGS} ${LDFLAGS} \
        `pkg-config --cflags gtk+-3.0` \
        `pkg-config --cflags webkit2gtk-4.1` \
        ${S}/mini-browser.c \
        `pkg-config --libs gtk+-3.0` \
        `pkg-config --libs webkit2gtk-4.1` \
        -o mini-browser
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/mini-browser ${D}${bindir}/
}

FILES:${PN} = "${bindir}/mini-browser"
```

----

If you want to compile locally in host system use below command

```bash
gcc `pkg-config --cflags gtk+-3.0` `pkg-config --cflags webkit2gtk-4.1` mini-browser.c `pkg-config --libs gtk+-3.0` `pkg-config --libs webkit2gtk-4.1` -o mini-browser.out
```

---
