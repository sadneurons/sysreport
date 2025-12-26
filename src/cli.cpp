#include "cli.h"
#include <iostream>
#include <algorithm>

bool hasFlag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

std::string getOptionValue(const std::vector<std::string>& args, const std::string& option) {
    auto it = std::find(args.begin(), args.end(), option);
    if (it != args.end() && ++it != args.end()) {
        return *it;
    }
    return "";
}

void printHelp() {
    std::cout << "Usage: " << PROGRAM_NAME << " [OPTIONS]\n\n"
              << "A comprehensive system monitoring utility for Linux.\n\n"
              << "Display Options:\n"
              << "  -h, --help          Show this help message\n"
              << "  -v, --version       Show version information\n"
              << "  -s, --static        Show static hardware information\n"
              << "  -d, --dynamic       Show dynamic utilization statistics\n"
              << "  -a, --all           Show both static and dynamic info (default)\n"
              << "\n"
              << "Filtering:\n"
              << "  --cpu-only          Show only CPU information\n"
              << "  --memory-only       Show only memory information\n"
              << "  --disk-only         Show only disk information\n"
              << "  --network-only      Show only network information\n"
              << "  --process-only      Show only process information\n"
              << "\n"
              << "Output Format:\n"
              << "  -f, --format FMT    Output format: text, json, csv (default: text)\n"
              << "  -o, --output FILE   Write output to file instead of stdout\n"
              << "  -c, --color         Enable colored output (default)\n"
              << "  --no-color          Disable colored output\n"
              << "  -p, --progress      Show progress bars for percentages\n"
              << "  -t, --timestamp     Include timestamp in output\n"
              << "  --alerts            Show threshold alerts/warnings\n"
              << "\n"
              << "Watch Mode:\n"
              << "  -w, --watch         Continuous monitoring mode\n"
              << "  -i, --interval SEC  Update interval in seconds (default: 2)\n"
              << "\n"
              << "Interactive Mode:\n"
              << "  -T, --tui           Launch interactive TUI mode (htop-like)\n"
              << "\n"
              << "History & Trends:\n"
              << "  --history           Show sparklines for recent metric history\n"
              << "  --baseline          Compare current metrics to baseline\n"
              << "  --save-baseline FILE Save current metrics as baseline\n"
              << "  --load-baseline FILE Load baseline from file for comparison\n"
              << "\n"
              << "Export Formats:\n"
              << "  --prometheus        Export metrics in Prometheus text format\n"
              << "  --influxdb          Export metrics in InfluxDB line protocol\n"
              << "\n"
              << "Daemon Mode:\n"
              << "  --daemon            Run as background daemon\n"
              << "  --daemon-log FILE   Daemon log file (default: /var/log/sysreport.log)\n"
              << "  --daemon-interval N Daemon polling interval in seconds (default: 60)\n"
              << "  --webhook URL       Send alerts to webhook URL\n"
              << "\n"
              << "Plugin System:\n"
              << "  --plugin FILE       Load a plugin from file (.so)\n"
              << "  --plugin-dir DIR    Load all plugins from directory\n"
              << "  --list-plugins      List loaded plugins and exit\n"
              << "\n"
              << "Configuration:\n"
              << "  --config FILE       Use custom config file\n"
              << "                      (default: ~/.config/sysreport/config.conf)\n"
              << "\n"
              << "Examples:\n"
              << "  " << PROGRAM_NAME << "                        # Show all system info\n"
              << "  " << PROGRAM_NAME << " -T                    # Launch interactive TUI\n"
              << "  " << PROGRAM_NAME << " -d -c -p              # Dynamic stats with colors and bars\n"
              << "  " << PROGRAM_NAME << " --cpu-only -w -i 1    # Watch CPU usage every second\n"
              << "  " << PROGRAM_NAME << " -s -f json -o hw.json # Save hardware info to JSON\n"
              << "  " << PROGRAM_NAME << " -d -t --alerts        # Dynamic with timestamps and alerts\n"
              << "  " << PROGRAM_NAME << " -w --history          # Watch mode with sparklines\n"
              << "  " << PROGRAM_NAME << " --plugin ./myplugin.so # Load a custom plugin\n"
              << "  " << PROGRAM_NAME << " --plugin-dir /opt/plugins # Load all plugins from dir\n"
              << "  " << PROGRAM_NAME << " --save-baseline base.dat # Save current state as baseline\n"
              << "  " << PROGRAM_NAME << " --baseline --load-baseline base.dat # Compare to baseline\n"
              << "  " << PROGRAM_NAME << " --prometheus          # Export metrics for Prometheus\n"
              << "  " << PROGRAM_NAME << " --influxdb            # Export in InfluxDB format\n"
              << "  " << PROGRAM_NAME << " --daemon --daemon-log /tmp/metrics.log # Run as daemon\n"
              << std::endl;
}

void printVersion() {
    std::cout << PROGRAM_NAME << " version " << VERSION << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
    std::cerr << "Try '" << PROGRAM_NAME << " --help' for more information." << std::endl;
}
