# This readme will be updated

## Build

```bash
gcc `pkg-config --cflags gtk+-3.0` `pkg-config --cflags webkit2gtk-4.0` tls-mini-browser.c `pkg-config --libs gtk+-3.0` `pkg-config --libs webkit2gtk-4.0` -o tls-mini-browser.out
```

> Run
>
> `./tls-mini-browser`


---
# Error in finding webkitgtk in pkg-config in st-image-weston

The .bb file uses webkitgtk-4.0, in st-image-weston the available package is webkitgtk-4.1, so use the below bb file to first check the available wenkitgtk version, then use the available pkg-config webkitgtk version in .bb

```bb
SUMMARY = "Mini Browser GTK3 WebKit2 Application with IGNORE TLS CA"
DESCRIPTION = "A minimal fullscreen browser using GTK3 and WebKit2"
LICENSE = "CLOSED"

SRC_URI = "file://tls-mini-browser.c"

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
        ${S}/tls-mini-browser.c \
        `pkg-config --libs gtk+-3.0` \
        `pkg-config --libs ${WEBKIT_PKG}` \
        -o tls-mini-browser
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/tls-mini-browser ${D}${bindir}/
}

FILES:${PN} = "${bindir}/tls-mini-browser"
```


So in my case it was webkitgtk-4.1, so for that here is the changed .bb file

```bb
SUMMARY = "Mini Browser GTK3 WebKit2 Application with IGNORE TLS CA"
DESCRIPTION = "A minimal fullscreen browser using GTK3 and WebKit2"
LICENSE = "CLOSED"

SRC_URI = "file://tls-mini-browser.c"

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
        ${S}/tls-mini-browser.c \
        `pkg-config --libs gtk+-3.0` \
        `pkg-config --libs webkit2gtk-4.1` \
        -o tls-mini-browser
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/tls-mini-browser ${D}${bindir}/
}

FILES:${PN} = "${bindir}/tls-mini-browser"
```

If you want to compile locally in host system use below command

```bash
gcc `pkg-config --cflags gtk+-3.0` `pkg-config --cflags webkit2gtk-4.1` tls-mini-browser.c `pkg-config --libs gtk+-3.0` `pkg-config --libs webkit2gtk-4.1` -o tls-mini-browser.out
```
