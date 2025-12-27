# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.7.0] - 2025-12-27

### Added - Production Security Features
- **GPG Signature Verification**: Plugins must be signed with GPG by default
  - Automatic .sig file detection and verification
  - Uses system `gpg` command for signature validation
  - `--no-verify-signatures` flag to disable (unsafe, development only)
  - Security events logged for signature failures
- **Root Execution Protection**: Prevents accidental root execution
  - Detects root privileges (uid=0 or euid=0)
  - Requires explicit `--allow-root` flag to proceed
  - Clear warning message with security explanation
- **Stricter Default Security**: Production-safe defaults
  - Security enforcement enabled by default (was disabled)
  - Default policy changed from "default" to "restricted"
  - Plugins sandboxed by default with minimal permissions
  - Resource limits enforced automatically

### Changed
- Default plugin security policy is now "restricted" (was more permissive)
- Plugin manager enables security enforcement by default
- Signature verification required unless explicitly disabled
- Help text updated with security notes and best practices

### Security
- **BREAKING**: Unsigned plugins will be rejected by default
- Root execution blocked unless explicitly allowed
- Tighter sandboxing for untrusted plugins
- Security audit trail for all validation events

### Documentation
- Added "SECURITY NOTES" section to help output
- Updated examples to show signed plugin usage
- Added warnings for unsafe flags (--no-verify-signatures, --allow-root)

## [0.6.0] - 2025-12-26

### Added
- **Security & Sandboxing**: Plugin security framework
  - SecurityManager class for plugin validation
  - Plugin path validation (prevent path traversal, check file permissions)
  - Plugin security policies (default, restricted, permissive)
  - Resource limits (memory, CPU, file descriptors)
  - Security event audit logging
  - `--plugin-security LEVEL` flag for security policy selection
  - `--no-plugin-sandbox` flag to disable sandboxing (unsafe mode)
  - PrivilegeGuard RAII class for temporary privilege elevation
  - World-writable plugin detection
  - .so file extension validation

### Security
- Plugins validated before loading
- Path traversal attempts blocked
- Security events logged to /var/log/sysreport-security.log
- Plugin security_validated flag tracked per plugin
- Support for future seccomp-bpf and capability dropping

### Technical
- Added security.h and security.cpp
- PluginManager integrated with SecurityManager
- Security validation in plugin loading pipeline
- Forward declarations for SecurityManager in plugin.h

## [0.5.0] - 2025-12-26

### Added
- **Plugin System**: Extensible architecture for custom metrics
  - Dynamic library loading using dlopen/dlsym
  - Plugin API with version checking (API v1)
  - PluginManager class for loading and managing plugins
  - `--plugin FILE` flag to load individual plugin .so files
  - `--plugin-dir DIR` flag to load all plugins from a directory
  - `--list-plugins` flag to display loaded plugin information
  - Helper macros for easy plugin development
  - Two example plugins:
    - System Uptime plugin: uptime metrics, boot time, idle time
    - Custom System Info plugin: user, hostname, shell, kernel, processes, datetime
  - Plugin metrics displayed in dedicated section with icon
  - Separate plugin build system (plugins/Makefile)
  - Plugin icon added to format.h (󰏖)

### Technical
- Shared library (.so) plugin architecture
- Plugin API version checking for compatibility
- Added -ldl flag to Makefile for dynamic linking

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
