# sysreport

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)](https://www.linux.org/)

A comprehensive, modern system monitoring utility for Linux that provides detailed hardware information and real-time system utilization statistics with multiple output formats and an intuitive command-line interface.

## ✨ Features

### 📊 Comprehensive Metrics
- **Hardware Information**: CPU model/cores, total RAM/swap, disk partitions, network interfaces, OS details
- **CPU Statistics**: Overall usage, per-core breakdown (all cores), load averages (1/5/15 min), temperature sensors
- **Memory Monitoring**: RAM and swap usage with available/used/percentage breakdowns
- **Disk Analytics**: Multi-partition support with usage statistics per mount point
- **Network Stats**: Traffic monitoring (RX/TX bytes) for all interfaces
- **Process Tracking**: Top 5 processes by memory consumption
- **System Info**: Uptime, thermal sensor data

### 🎨 Flexible Output
- **Multiple Formats**: Text (human-readable), JSON (machine-parseable), CSV (spreadsheet-ready)
- **Visual Enhancements**:
  - Color-coded thresholds (🟢 <70%, 🟡 70-89%, 🔴 ≥90%)
  - Progress bars for percentage values
  - Timestamps for logging
  - Alert notifications for critical thresholds

### 🔍 Advanced Features
- **Watch Mode**: Continuous monitoring with customizable refresh intervals
- **Smart Filtering**: Display only CPU, memory, disk, network, or process information
- **File Output**: Save reports to files for analysis or logging
- **Configurable Display**: Toggle colors, progress bars, timestamps, and alerts

## 📸 Example Output

**Full System Report with Enhancements:**
```
Timestamp: 2025-12-26 21:06:47

=== HARDWARE INFORMATION ===
OS:         Ubuntu 24.04.3 LTS
CPU Model:  Intel(R) Core(TM) i9-14900KS
CPU Cores:  32
Total RAM:  96382 MB
Total Swap: 8191 MB
Disks:      2 partition(s)
  - / (/dev/nvme0n1p2): 3665 GB
  - /boot/efi (/dev/nvme0n1p1): 1 GB
Network:    enp7s0, wlo1

=== UTILIZATION ===
CPU Usage:  9.1% [█░░░░░░░░░░░░░░░░░░░]
Per-Core:      0%    0%    9%    0%   10%    0%    0%   11% 
              20%    0%   22%    0%   11%    0%    9%    0% 
               0%   22%   10%   10%    0%    0%    0%    9% 
               0%    0%    0%    0%    0%    0%    0%    0%
Load Avg:   1.04, 1.10, 1.05
Uptime:     10h 25m
Temp:       27.8°C, 32.0°C, 29.0°C

RAM Used:   14041 MB / 14.6% [██░░░░░░░░░░░░░░░░░░]
RAM Avail:  82341 MB

Disk /:
  Used:     1341 GB / 36.6% [███████░░░░░░░░░░░░░]
  Available: 2324 GB

Network:
  enp7s0    : RX          0 MB  TX          0 MB
  wlo1      : RX      10160 MB  TX        591 MB

Top Processes (by memory):
    60658  code                             1432 MB
    60162  code                             1430 MB
    73844  chrome                           1429 MB
```

**JSON Output Example:**
```json
{
  "timestamp": "2025-12-26 21:06:47",
  "hardware": {
    "os": "Ubuntu 24.04.3 LTS",
    "cpu_model": "Intel(R) Core(TM) i9-14900KS",
    "cpu_cores": 32,
    "total_ram_mb": 96382
  },
  "utilization": {
    "cpu_percent": 9.1,
    "load_avg": [1.04, 1.10, 1.05],
    "ram_percent": 14.6
  }
}
```

## 🚀 Quick Start

### Option 1: Download Pre-built Package (Recommended)

```bash
# Download the latest release
wget https://github.com/YOUR_USERNAME/sysreport/releases/download/v0.1.0/sysreport_0.1.0_amd64.deb

# Install
sudo dpkg -i sysreport_0.1.0_amd64.deb

# Run
sysreport --help
```

### Option 2: Build from Source

```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/sysreport.git
cd sysreport

# Build
make

# Run
./build/main --help

# Install system-wide (optional)
sudo make install
```

### Option 3: Build Debian Package

```bash
git clone https://github.com/YOUR_USERNAME/sysreport.git
cd sysreport
./build-deb.sh
sudo dpkg -i sysreport_0.1.0_amd64.deb
```

## 📖 Usage

### Basic Commands

```bash
# Show all system information (default)
sysreport

# Show only dynamic statistics
sysreport -d

# Show only hardware info
sysreport -s

# Display help
sysreport --help
```

### Visual Enhancements

```bash
# With colors and progress bars
sysreport -d -c -p

# Add timestamps and threshold alerts
sysreport -d -t --alerts

# Full enhancements
sysreport -a -c -p -t --alerts
```

### Filtering

```bash
# CPU information only
sysreport --cpu-only

# Memory statistics only
sysreport --memory-only

# Disk usage only
sysreport --disk-only
```

### Watch Mode (Real-time Monitoring)

```bash
# Continuous monitoring (2-second intervals)
sysreport -w

# Update every second
sysreport -w -i 1

# Watch CPU with visual enhancements
sysreport --cpu-only -w -i 1 -c -p
```

### Output Formats

```bash
# JSON format
sysreport -f json

# CSV format (for spreadsheets)
sysreport -f csv

# Save to file
sysreport -d -o report.txt

# JSON to file
sysreport -s -f json -o hardware.json
```

### Advanced Examples

```bash
# Complete system report with all enhancements
sysreport -a -c -p -t --alerts -o system_report.txt

# Monitor CPU and save JSON snapshots
sysreport --cpu-only -f json -o cpu_stats.json

# Watch all metrics with colored output
sysreport -w -i 2 -c -p --alerts
```

## 🛠️ Command-Line Options

### Display Options
- `-h, --help` - Show help message
- `-v, --version` - Show version information
- `-s, --static` - Show static hardware info
- `-d, --dynamic` - Show dynamic utilization
- `-a, --all` - Show both (default)

### Filtering Options
- `--cpu-only` - CPU information only
- `--memory-only` - Memory information only
- `--disk-only` - Disk information only
- `--network-only` - Network information only
- `--process-only` - Process list only

### Output Options
- `-f, --format FORMAT` - Output format: text, json, csv
- `-o, --output FILE` - Write to file
- `-c, --color` - Enable colors (default)
- `--no-color` - Disable colors
- `-p, --progress` - Show progress bars
- `-t, --timestamp` - Include timestamp
- `--alerts` - Show threshold warnings

### Watch Mode
- `-w, --watch` - Continuous monitoring
- `-i, --interval SEC` - Update interval (default: 2)

## 📋 Requirements

- **OS**: Linux (any distribution with `/proc` filesystem)
- **Compiler**: g++ with C++17 support
- **Build Tools**: make
- **Optional**: dpkg-deb (for building .deb packages)

## 🔧 Building

```bash
# Build the executable
make

# Clean build artifacts
make clean

# Install system-wide
sudo make install

# Uninstall
sudo make uninstall

# Build .deb package
./build-deb.sh
```

## 📚 Documentation

View the comprehensive man page after installation:
```bash
man sysreport
```

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🗺️ Roadmap

- [ ] GPU statistics (NVIDIA/AMD)
- [ ] Battery information for laptops
- [ ] Historical data tracking
- [ ] Configuration file support
- [ ] Plugin system for custom metrics

## 🙏 Acknowledgments

- Built with modern C++17
- Inspired by tools like `htop`, `top`, and `neofetch`

---

**Made with ❤️ for the Linux community**
