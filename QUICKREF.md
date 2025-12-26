# sysreport Quick Reference

## Common Commands

```bash
sysreport                          # Show everything
sysreport -d                       # Dynamic stats only
sysreport -s                       # Hardware info only
sysreport -d -c -p                 # Dynamic with colors & progress bars
sysreport -w                       # Watch mode (refresh every 2s)
sysreport -w -i 1                  # Watch mode (refresh every 1s)
sysreport --cpu-only               # CPU stats only
sysreport --memory-only            # Memory stats only
sysreport -f json                  # JSON output
sysreport -f csv                   # CSV output
sysreport -o report.txt            # Save to file
sysreport -d -t --alerts           # With timestamp & alerts
man sysreport                      # Full documentation
```

## Installation

```bash
# From .deb package
sudo dpkg -i sysreport_0.1.0_amd64.deb

# From source
make && sudo make install

# Build .deb yourself
./build-deb.sh
```

## Output Formats

- `text` - Human-readable (default)
- `json` - Machine-parseable
- `csv` - Spreadsheet-ready

## Color Codes

- 🟢 Green: <70%
- 🟡 Yellow: 70-89%
- 🔴 Red: ≥90%
