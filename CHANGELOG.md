# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Beautiful box-drawing table formatting for process display
- Unicode icons for all major sections (CPU, RAM, disk, network, processes, etc.)
- Automatic terminal width detection for responsive output
- Professional section separators with box-drawing characters
- Improved column alignment and visual hierarchy

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
