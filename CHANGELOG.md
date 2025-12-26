# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.4.0] - 2025-12-26

### Added
- **Prometheus Exporter**: Export metrics in Prometheus text format
  - Full metric labels and help text
  - CPU, memory, disk, network, GPU, battery metrics
  - Per-core CPU usage with core labels
  - Compatible with Prometheus scraping
  - `--prometheus` flag for instant export
- **InfluxDB Exporter**: Export metrics in InfluxDB line protocol
  - Nanosecond precision timestamps
  - Tags for dimensional data (interface, device, mount point)
  - Fields for all metric values
  - `--influxdb` flag for instant export
- **Daemon Mode**: Run sysreport as a background daemon
  - Continuous logging to file
  - Configurable polling interval (default: 60s)
  - Automatic PID file management
  - Support for multiple export formats (JSON, CSV, Prometheus, InfluxDB)
  - `--daemon` flag with optional `--daemon-log` and `--daemon-interval`
- **Webhook Integration**: Send alerts to HTTP endpoints
  - Alert on threshold violations (CPU, memory, disk, GPU)
  - JSON payload with metric, value, threshold, severity
  - Configurable webhook URL via `--webhook`
  - Automatic hostname inclusion in alerts
- New CLI flags documented in help text with examples
- DaemonConfig structure for daemon configuration
- WebhookClient class for HTTP POST requests

### Changed
- Version bumped to 0.4.0
- Updated help text with Export Formats and Daemon Mode sections
- Enhanced main.cpp with exporter and daemon handling

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
