# sysreport Development TODO

## Current Status: v0.5.0 - Stage 8 Complete ✅

**Last Updated:** December 26, 2025  
**Current Version:** 0.5.0  
**Current Stage:** Stage 9 (Security & Sandboxing) - IN PROGRESS

### Quick Resume Context
- All stages 1-8 are complete and committed to git
- Plugin system (Stage 8) just implemented with dynamic loading
- Two example plugins working: uptime.so, custom.so
- Next: Implement security features and plugin sandboxing

---

## Completed Stages

### Stage 1: Better Output Formatting ✅ (v0.1.0)
- [x] Add box drawing characters for tables
- [x] Align columns properly
- [x] Terminal width auto-detection
- [x] Add Unicode icons (CPU 󰻠, RAM 󰍛, disk 󰋊, network 󰖩)
- [x] Improve section separators

### Stage 2: Configuration File Support ✅ (v0.1.0)
- [x] Create config file parser (TOML or INI format)
- [x] Support ~/.config/sysreport/config.conf
- [x] Default display options (colors, progress bars, format)
- [x] Custom thresholds for alerts
- [x] Save favorite flags

### Stage 3: Extended Metrics ✅ (v0.1.0)
- [x] GPU stats (NVIDIA via nvidia-smi)
- [x] GPU stats (AMD via radeontop)
- [x] Battery status and health (for laptops)
- [x] Network speed calculation (delta/Mbps, not just totals)
- [x] Fan speeds (if available)

### Stage 4: Marketing Materials ✅ (v0.1.0)
- [x] Add comparison table to README (vs top, htop, glances)
- [x] Create use case examples section
- [x] Add FAQ to README
- [ ] Create animated GIF demo with asciinema (optional)
- [ ] Take real terminal screenshots (optional)

### Stage 5: Interactive TUI Mode ✅ (v0.2.0)
- [x] Design ncurses-based interface layout
- [x] Implement window/panel management
- [x] Add real-time auto-refresh display
- [x] Keyboard navigation (arrows, tab, Enter)
- [x] Process list with sorting (CPU, memory, PID, name)
- [x] Process filtering by name/user
- [x] Color scheme support
- [x] Help screen with key bindings
- [x] Kill process functionality
- [x] Resize handling

### Stage 6: Historical Data & Trends ✅ (v0.3.0)
- [x] Track metrics over time
- [x] Sparklines for recent history (▁▂▃▅▇)
- [x] Comparison mode (vs baseline)
- [x] Show trends (increasing/decreasing arrows)
- [x] Save/load baseline files
- [x] Linear regression for trend calculation
- [x] CLI flags: --history, --baseline, --save-baseline, --load-baseline

### Stage 7: Advanced Integrations ✅ (v0.4.0)
- [x] Prometheus exporter format (text format with metrics, labels, help text)
- [x] InfluxDB output format (line protocol with tags and timestamps)
- [x] Daemon mode for continuous logging
- [x] Webhook support for alerts
- [x] Configurable export formats in daemon mode
- [x] Alert thresholds for CPU, memory, disk, GPU
- [x] CLI flags: --daemon, --prometheus, --influxdb, --webhook

### Stage 8: Plugin System ✅ (v0.5.0)
- [x] Design plugin architecture
- [x] Shared library loading (.so files)
- [x] Plugin API documentation (docs/PLUGIN_DEVELOPMENT.md)
- [x] Example plugins (uptime.so, custom.so)
- [x] Plugin manager implementation (PluginManager class)
- [x] CLI integration (--plugin, --plugin-dir, --list-plugins)
- [x] Helper macros (DECLARE_PLUGIN, etc.)
- [x] API version checking

---

## In Progress

### Stage 9: Security & Sandboxing 🚧 (v0.6.0 target)
- [ ] Plugin permission system
- [ ] Sandboxed plugin execution (seccomp-bpf)
- [ ] Plugin signature verification
- [ ] Capability dropping for daemon mode
- [ ] Read-only filesystem access for plugins
- [ ] Resource limits (CPU, memory, file descriptors)
- [ ] Audit logging for security events
- [ ] Safe configuration file parsing
- [ ] Input validation and sanitization

---

## Upcoming Stages

### Stage 10: Cloud Integration (v0.7.0 target)
- [ ] AWS CloudWatch integration
- [ ] Azure Monitor integration
- [ ] Google Cloud Monitoring integration
- [ ] Upload metrics to cloud storage (S3, GCS, Azure Blob)
- [ ] Cloud authentication (IAM, service accounts)
- [ ] Encrypted metric transmission

### Stage 11: AI/ML Features (v0.8.0 target)
- [ ] Anomaly detection using statistical models
- [ ] Predictive alerts (forecast resource exhaustion)
- [ ] Pattern recognition in metrics
- [ ] Auto-tuning thresholds
- [ ] Clustering similar behavior patterns
- [ ] Time series forecasting

### Stage 12: Enterprise Features (v0.9.0 target)
- [ ] Multi-host monitoring
- [ ] Centralized dashboard
- [ ] Role-based access control (RBAC)
- [ ] Compliance reporting (SOC2, HIPAA)
- [ ] SLA tracking and reporting
- [ ] Integration with ticketing systems
- [ ] Custom report generation

---

## Backlog (Future Ideas)

- [ ] Remote monitoring (SSH)
- [ ] Docker container stats
- [ ] Systemd service monitoring
- [ ] Desktop notifications
- [ ] Email alerts
- [ ] Color themes (dark/light/solarized)
- [ ] Custom units (MB/GiB, C/F)
- [ ] Multi-language support
- [ ] Web interface mode
- [ ] Mobile app companion
- [ ] GraphQL API
- [ ] Kubernetes integration

---
