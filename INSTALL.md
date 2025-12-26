# Installation Instructions

## Quick Install (Debian/Ubuntu)

Download and install the latest release:

```bash
# Download the .deb package
wget https://github.com/YOUR_USERNAME/sysreport/releases/download/v0.1.0/sysreport_0.1.0_amd64.deb

# Install
sudo dpkg -i sysreport_0.1.0_amd64.deb

# If dependencies are missing, run:
sudo apt-get install -f
```

## Manual Install from Source

```bash
git clone https://github.com/YOUR_USERNAME/sysreport.git
cd sysreport
make
sudo make install  # or use ./build-deb.sh to create package
```

## Usage

After installation, run:
```bash
sysreport --help
man sysreport
```

## Uninstall

```bash
sudo dpkg -r sysreport
```
