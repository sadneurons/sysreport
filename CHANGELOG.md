# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.3.0] - 2025-12-26

### Added
- **Historical Data & Trends**: Track metrics over time and visualize trends
  - Sparkline charts showing recent metric history (▁▂▃▅▇)
  - Trend indicators with arrows (↑ rising, ↓ falling, → stable)
  - Linear regression-based trend calculation
  - Baseline snapshot capture and comparison
  - Save/load baseline metrics to/from file
  - Delta comparison vs baseline (CPU, RAM)
  - CSV export format for historical data
  - New CLI flags: `--history`, `--baseline`, `--save-baseline FILE`, `--load-baseline FILE`
- MetricHistory class for time-series data storage with rolling window
- Sparkline rendering using Unicode block characters

### Changed
- Updated help text with history and baseline examples
- DisplayOptions struct now includes history-related fields

## [0.2.0] - 2025-12-26

### Added
- **Interactive TUI Mode**: Full-featured ncurses interface (like htop)
  - Real-time system monitoring dashboard
  - Sortable process list (CPU, memory, PID, name)
  - Keyboard navigation and process management
  - Kill process functionality
  - Help screen with key bindings
  - Window resize handling
- Beautiful box-drawing table formatting for process display
- Unicode icons for all major sections (CPU, RAM, disk, network, processes, etc.)
- Automatic terminal width detection for responsive output
- Professional section separators with box-drawing characters
- Improved column alignment and visual hierarchy
- Configuration file support (~/.config/sysreport/config.conf)
- Custom threshold settings for alerts
- GPU monitoring (NVIDIA and AMD support)
- Battery status and health monitoring for laptops
- Network speed calculation (real-time Mbps)
- Fan speed monitoring

### Fixed
- UTF-8 encoding issues with box-drawing characters

## [0.1.0] - 2025-12-26

### Added
- Initial release of sysreport
- Hardware information display (CPU, RAM, swap, disks, network interfaces)
- Dynamic system utilization monitoring (CPU, memory, disk, network, processes)
- Per-core CPU usage breakdown
- Temperature sensor monitoring
- Top 5 processes by memory usage
- System uptime and load averages
- Multiple output formats (text, JSON, CSV)
- Colored output with configurable thresholds
- Progress bars for percentage values
- Timestamp support
- Threshold alerts and warnings
- Watch mode with configurable refresh intervals
- Filtering options (CPU-only, memory-only, disk-only, network-only, process-only)
- Command-line options for flexible output control
- Man page documentation
- Debian package (.deb) support
