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
              << "Configuration:\n"
              << "  --config FILE       Use custom config file\n"
              << "                      (default: ~/.config/sysreport/config.conf)\n"
              << "\n"
              << "Examples:\n"
              << "  " << PROGRAM_NAME << "                        # Show all system info\n"
              << "  " << PROGRAM_NAME << " -d -c -p              # Dynamic stats with colors and bars\n"
              << "  " << PROGRAM_NAME << " --cpu-only -w -i 1    # Watch CPU usage every second\n"
              << "  " << PROGRAM_NAME << " -s -f json -o hw.json # Save hardware info to JSON\n"
              << "  " << PROGRAM_NAME << " -d -t --alerts        # Dynamic with timestamps and alerts\n"
              << std::endl;
}

void printVersion() {
    std::cout << PROGRAM_NAME << " version " << VERSION << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
    std::cerr << "Try '" << PROGRAM_NAME << " --help' for more information." << std::endl;
}
