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
- **Network Stats**: Traffic monitoring (RX/TX bytes) with real-time speed calculation (Mbps)
- **GPU Monitoring**: NVIDIA and AMD GPU usage, memory, and temperature
- **Battery Info**: Status, charge level, and health percentage (laptops)
- **Fan Speeds**: RPM monitoring for system cooling fans
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
- **Configuration File**: Persistent settings via `~/.config/sysreport/config.conf`
- **Watch Mode**: Continuous monitoring with customizable refresh intervals
- **Smart Filtering**: Display only CPU, memory, disk, network, or process information
- **File Output**: Save reports to files for analysis or logging
- **Configurable Display**: Toggle colors, progress bars, timestamps, and alerts
- **Custom Thresholds**: Set your own warning/critical levels for alerts

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

### Configuration
- `--config FILE` - Use custom config file (default: `~/.config/sysreport/config.conf`)

## ⚙️ Configuration File

sysreport supports persistent configuration via `~/.config/sysreport/config.conf`. This allows you to set defaults without using command-line flags every time.

### Creating Your Config

```bash
# Create config directory
mkdir -p ~/.config/sysreport

# Copy example config
cp config.conf.example ~/.config/sysreport/config.conf

# Edit with your preferred settings
nano ~/.config/sysreport/config.conf
```

### Example Configuration

```ini
[display]
# Visual settings
colors = true
progress_bars = true
alerts = true
timestamp = false
format = text
top_processes = 5

[watch]
# Default refresh interval for watch mode
default_interval = 2

[thresholds]
# Custom alert thresholds (percentage)
cpu_warning = 70.0
cpu_critical = 85.0
ram_warning = 75.0
ram_critical = 85.0
disk_warning = 80.0
disk_critical = 90.0
temp_warning = 60.0
temp_critical = 80.0

[filters]
# Default filters (normally false)
cpu_only = false
memory_only = false
```

**Note**: Command-line options always override config file settings.

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

## 📊 Comparison with Other Tools

| Feature | sysreport | top | htop | glances |
|---------|-----------|-----|------|---------|
| **Installation** | Single binary | Built-in | apt install | pip install |
| **Dependencies** | None | None | ncurses | Python + libs |
| **Size** | ~500KB | N/A | ~200KB | ~50MB |
| **GPU Monitoring** | ✅ NVIDIA/AMD | ❌ | ❌ | ✅ |
| **Battery Info** | ✅ | ❌ | ❌ | ✅ |
| **Network Speeds** | ✅ Real-time | ❌ | ❌ | ✅ |
| **Fan Speeds** | ✅ | ❌ | ❌ | ✅ |
| **Per-Core CPU** | ✅ All cores | ✅ | ✅ | ✅ |
| **Output Formats** | Text/JSON/CSV | Text only | Text only | Text/JSON |
| **Config File** | ✅ INI format | ❌ | ❌ | ✅ INI |
| **Colors** | ✅ Customizable | Limited | ✅ | ✅ |
| **Progress Bars** | ✅ | ❌ | ✅ | ✅ |
| **Watch Mode** | ✅ Configurable | ✅ | Interactive | Interactive |
| **File Output** | ✅ | ❌ | ❌ | ✅ |
| **Scriptable** | ✅ JSON/CSV | ❌ | ❌ | ✅ |
| **Interactive TUI** | 🚧 Coming | ✅ | ✅ | ✅ |
| **Resource Usage** | Very Low | Very Low | Low | Medium |

### Why Choose sysreport?

- **Lightweight**: No Python dependencies, minimal resource usage
- **Modern**: Beautiful Unicode output with icons and tables
- **Flexible**: Multiple output formats for scripting and logging
- **Comprehensive**: GPU, battery, fans - not just CPU/RAM
- **Configurable**: Persistent settings via config file
- **Developer-Friendly**: Clean JSON/CSV output for automation

## 💡 Use Cases

### System Administration

```bash
# Quick health check
sysreport --alerts

# Monitor server resources
sysreport -w -i 5 --no-static

# Generate daily report
sysreport -f json -o /var/log/sysreport-$(date +%Y%m%d).json
```

### Development & Debugging

```bash
# Watch resource usage during builds
sysreport -w --cpu-only --memory-only

# Profile application impact
sysreport -w -i 1 --process-only

# Export for analysis
sysreport -f csv -o metrics.csv
```

### DevOps & Automation

```bash
# Pre-deployment health check
if sysreport -f json | jq -e '.utilization.cpu_percent > 80'; then
    echo "High CPU - defer deployment"
fi

# Cron job for monitoring
*/15 * * * * sysreport -f json >> /var/log/system-metrics.jsonl

# Alert on disk space
sysreport --disk-only | grep -q "WARNING" && notify-send "Low disk space"
```

### Gaming & Workstations

```bash
# Monitor GPU during gaming
sysreport -w -i 2 --gpu-only

# Battery monitoring on laptop
sysreport -w --battery-only

# Thermal monitoring
sysreport -w --alerts -p
```

### Data Center & Cloud

```bash
# Export to monitoring dashboard
sysreport -f json | curl -X POST https://monitoring.example.com/metrics

# Multi-server reporting
for server in $(cat servers.txt); do
    ssh $server sysreport -f csv >> cluster-metrics.csv
done

# Resource threshold alerts
sysreport --alerts --cpu-only --memory-only
```

## ❓ FAQ

### General Questions

**Q: What Linux distributions are supported?**  
A: Any distribution with a `/proc` filesystem. Tested on Ubuntu, Debian, Fedora, Arch, and RHEL.

**Q: Does it require root/sudo?**  
A: No! sysreport runs with normal user permissions. Some hwmon sensors may require root for access.

**Q: How much overhead does it add?**  
A: Minimal - typically <0.1% CPU and ~5MB RAM. Designed to be lightweight.

**Q: Can I use it in scripts?**  
A: Yes! Use JSON or CSV format for easy parsing: `sysreport -f json | jq '.utilization.cpu_percent'`

### Configuration

**Q: Where is the config file?**  
A: `~/.config/sysreport/config.conf` - see example: `cp /usr/share/sysreport/config.conf.example ~/.config/sysreport/config.conf`

**Q: How do I change alert thresholds?**  
A: Edit the `[thresholds]` section in your config file or use `--config` to specify a custom file.

**Q: Can I disable colors permanently?**  
A: Yes, set `colors = false` in the `[display]` section of your config.

### GPU & Hardware

**Q: My GPU isn't detected. What's wrong?**  
A: For NVIDIA, ensure `nvidia-smi` is installed. For AMD, check `/sys/class/drm/card0/device/` exists.

**Q: Battery shows as not present on my laptop?**  
A: Check `/sys/class/power_supply/BAT0/` exists. Some laptops use BAT1 or have different naming.

**Q: Can I monitor multiple GPUs?**  
A: Yes! sysreport automatically detects all NVIDIA and AMD GPUs.

### Output & Formatting

**Q: Can I export to Prometheus/InfluxDB?**  
A: Not yet, but it's on the roadmap (Stage 7). Use JSON output with a converter for now.

**Q: The Unicode icons don't display correctly?**  
A: Ensure your terminal supports UTF-8 and has a Nerd Font installed (FiraCode, JetBrains Mono, etc.).

**Q: How do I get CSV with headers?**  
A: CSV format includes headers automatically. Use `sysreport -f csv` and pipe to any spreadsheet tool.

### Troubleshooting

**Q: "Command not found" after installation?**  
A: Run `hash -r` to refresh your shell's command cache, or restart your terminal.

**Q: Network speeds show 0.00 Mbps?**  
A: Speeds are calculated between samples. Run in watch mode (`-w`) or wait for the second reading.

**Q: Temperatures not showing?**  
A: Thermal sensors depend on hardware support. Check `ls /sys/class/hwmon/` for available sensors.

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🗺️ Roadmap

- [x] GPU statistics (NVIDIA/AMD)
- [x] Battery information for laptops
- [x] Configuration file support
- [x] Network speed calculation
- [ ] Interactive TUI mode (ncurses)
- [ ] Historical data tracking with sparklines
- [ ] Plugin system for custom metrics
- [ ] Prometheus/InfluxDB exporters

## 🙏 Acknowledgments

- Built with modern C++17
- Inspired by tools like `htop`, `top`, `glances`, and `neofetch`
- Community feedback and contributions

---

**Made with ❤️ for the Linux community**
