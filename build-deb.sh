#!/bin/bash

# Build the project
echo "Building sysreport..."
VERSION="0.6.0"
make clean
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Create debian package structure
echo "Creating .deb package structure..."
mkdir -p debian/usr/bin
mkdir -p debian/usr/share/doc/sysreport
mkdir -p debian/usr/share/sysreport
mkdir -p debian/usr/share/man/man1
mkdir -p debian/DEBIAN

# Copy the binary
cp build/main debian/usr/bin/sysreport
chmod 755 debian/usr/bin/sysreport

# Copy documentation
cp README.md debian/usr/share/doc/sysreport/

# Copy example config
cp config.conf.example debian/usr/share/sysreport/

# Copy and compress man page
cp sysreport.1 debian/usr/share/man/man1/sysreport.1
gzip -9 -f debian/usr/share/man/man1/sysreport.1

# Create control file
cat > debian/DEBIAN/control << EOF
Package: sysreport
Version: 0.4.0
Section: utils
Priority: optional
Architecture: amd64
Depends: libc6 (>= 2.34), libstdc++6 (>= 11), libncurses6 (>= 6)
Maintainer: Your Name <you@example.com>
Description: System monitoring and reporting tool
 A comprehensive command-line tool for monitoring system vital statistics
 including CPU, memory, disk, network, GPU, battery, and process information.
 .
 Features include:
  - Interactive TUI mode (htop-like interface)
  - Real-time system monitoring
  - GPU monitoring (NVIDIA/AMD)
  - Battery status and health
  - Multiple output formats (text, JSON, CSV)
  - Watch mode with configurable intervals
  - Colored output with progress bars
  - Filtering options for specific metrics
EOF

# Build the package
echo "Building .deb package..."
dpkg-deb --build debian sysreport_0.4.0_amd64.deb

if [ $? -eq 0 ]; then
    echo "Successfully created sysreport_0.4.0_amd64.deb"
    echo ""
    echo "To install, run:"
    echo "  sudo dpkg -i sysreport_0.4.0_amd64.deb"
    echo ""
    echo "To uninstall, run:"
    echo "  sudo dpkg -r sysreport"
else
    echo "Package build failed!"
    exit 1
fi
