#!/bin/bash

# Build the project
echo "Building sysreport..."
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
mkdir -p debian/usr/share/man/man1
mkdir -p debian/DEBIAN

# Copy the binary
cp build/main debian/usr/bin/sysreport
chmod 755 debian/usr/bin/sysreport

# Copy documentation
cp README.md debian/usr/share/doc/sysreport/

# Copy and compress man page
cp sysreport.1 debian/usr/share/man/man1/sysreport.1
gzip -9 -f debian/usr/share/man/man1/sysreport.1

# Update control file if needed (already created)
# The control file should already exist in debian/DEBIAN/control

# Build the package
echo "Building .deb package..."
dpkg-deb --build debian sysreport_0.1.0_amd64.deb

if [ $? -eq 0 ]; then
    echo "Successfully created sysreport_0.1.0_amd64.deb"
    echo ""
    echo "To install, run:"
    echo "  sudo dpkg -i sysreport_0.1.0_amd64.deb"
    echo ""
    echo "To uninstall, run:"
    echo "  sudo dpkg -r sysreport"
else
    echo "Package build failed!"
    exit 1
fi
