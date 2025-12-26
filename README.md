# SysReport - System Monitoring Tool

A comprehensive Linux system monitoring utility with support for hardware info, real-time utilization stats, multiple output formats, and watch mode.

## Features

### Metrics
- **Hardware Info**: CPU model/cores, RAM, swap, disks, network interfaces, OS
- **CPU Stats**: Overall usage, per-core breakdown, load average, temperatures
- **Memory**: RAM usage (used/available/%), swap usage
- **Disk**: Multiple partitions, usage per mount point
- **Network**: Traffic stats (RX/TX) per interface
- **Processes**: Top 5 processes by memory
- **System**: Uptime, thermal sensors

### Output Options
- **Formats**: Text (default), JSON, CSV
- **Enhancements**: 
  - Color-coded thresholds (green <70%, yellow <90%, red >=90%)
  - Progress bars for percentages
  - Timestamps
  - Threshold alerts/warnings
- **Filtering**: Show only CPU, memory, disk, network, or process info

### Watch Mode
- Continuous monitoring with customizable refresh intervals
- Clear screen and update in-place

## Installation

### From .deb Package

```bash
./build-deb.sh                        # Build the package
sudo dpkg -i sysreport_0.1.0_amd64.deb  # Install
```

After installation, run from anywhere:
```bash
sysreport --help
```

### From Source

```bash
make                  # Build
./build/main --help   # Run
```

## Usage

### Basic Examples

```bash
sysreport                    # Show all info (text format)
sysreport -d                 # Dynamic stats only
sysreport -s                 # Hardware info only
sysreport -h                 # Show help
```

### With Enhancements

```bash
sysreport -d -c -p              # Colors + progress bars
sysreport -d -t --alerts        # Timestamps + threshold alerts
sysreport --cpu-only -c -p      # CPU only with visual enhancements
```

### Watch Mode

```bash
sysreport -w                    # Watch mode (2 sec intervals)
sysreport -w -i 1               # Update every second
sysreport --cpu-only -w -i 1    # Watch CPU only
sysreport --memory-only -w      # Watch memory only
```

### Output Formats

```bash
sysreport -f json               # JSON format
sysreport -f csv                # CSV format
sysreport -d -o stats.txt       # Save to file
sysreport -s -f json -o hw.json # Hardware as JSON
```

### Filtering

```bash
sysreport --cpu-only            # CPU stats only
sysreport --memory-only         # Memory stats only
sysreport --disk-only           # Disk stats only
sysreport --network-only        # Network stats only
sysreport --process-only        # Process list only
```

## Command-Line Options

### Display Options
- `-h, --help` - Show help message
- `-v, --version` - Show version
- `-s, --static` - Show static hardware info
- `-d, --dynamic` - Show dynamic utilization
- `-a, --all` - Show both (default)

### Filtering
- `--cpu-only` - CPU information only
- `--memory-only` - Memory information only
- `--disk-only` - Disk information only
- `--network-only` - Network information only
- `--process-only` - Process list only

### Output Format
- `-f, --format FMT` - Format: text, json, csv
- `-o, --output FILE` - Write to file
- `-c, --color` - Enable colors (default for text)
- `--no-color` - Disable colors
- `-p, --progress` - Show progress bars
- `-t, --timestamp` - Include timestamp
- `--alerts` - Show threshold warnings

### Watch Mode
- `-w, --watch` - Continuous monitoring
- `-i, --interval SEC` - Update interval (default: 2)

## Requirements

- g++ with C++17 support
- make
- Linux kernel with /proc filesystem
- dpkg-deb (for creating .deb packages)

## Uninstall

```bash
sudo dpkg -r sysreport
```

## Development

```bash
make clean          # Clean build artifacts
make                # Build
make run            # Build and run
```