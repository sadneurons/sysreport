#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include "cli.h"
#include "system_info.h"

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::vector<std::string> args(argv + 1, argv + argc);
    
    // Handle help flag
    if (hasFlag(args, "-h") || hasFlag(args, "--help")) {
        printHelp();
        return 0;
    }
    
    // Handle version flag
    if (hasFlag(args, "-v") || hasFlag(args, "--version")) {
        printVersion();
        return 0;
    }
    
    // Setup display options
    DisplayOptions opts;
    
    // Determine what to show
    opts.show_static = hasFlag(args, "-s") || hasFlag(args, "--static");
    opts.show_dynamic = hasFlag(args, "-d") || hasFlag(args, "--dynamic");
    bool show_all = hasFlag(args, "-a") || hasFlag(args, "--all");
    
    // Default: show all if nothing specified
    if (!opts.show_static && !opts.show_dynamic && !show_all) {
        show_all = true;
    }
    
    if (show_all) {
        opts.show_static = true;
        opts.show_dynamic = true;
    }
    
    // Filtering options
    opts.cpu_only = hasFlag(args, "--cpu-only");
    opts.memory_only = hasFlag(args, "--memory-only");
    opts.disk_only = hasFlag(args, "--disk-only");
    opts.network_only = hasFlag(args, "--network-only");
    opts.process_only = hasFlag(args, "--process-only");
    
    // If any filter is set, only show dynamic
    if (opts.cpu_only || opts.memory_only || opts.disk_only || 
        opts.network_only || opts.process_only) {
        opts.show_dynamic = true;
        opts.show_static = false;
    }
    
    // Display options
    opts.use_colors = !hasFlag(args, "--no-color");
    if (hasFlag(args, "-c") || hasFlag(args, "--color")) {
        opts.use_colors = true;
    }
    
    opts.show_progress_bars = hasFlag(args, "-p") || hasFlag(args, "--progress");
    opts.show_timestamp = hasFlag(args, "-t") || hasFlag(args, "--timestamp");
    opts.show_alerts = hasFlag(args, "--alerts");
    
    // Get format
    opts.format = getOptionValue(args, "-f");
    if (opts.format.empty()) {
        opts.format = getOptionValue(args, "--format");
    }
    if (opts.format.empty()) {
        opts.format = "text";
    }
    
    // Validate format
    if (opts.format != "text" && opts.format != "json" && opts.format != "csv") {
        printError("Invalid format: " + opts.format + ". Use text, json, or csv.");
        return 1;
    }
    
    // Disable colors for non-text formats
    if (opts.format != "text") {
        opts.use_colors = false;
    }
    
    // Get output file if specified
    std::string output_file = getOptionValue(args, "-o");
    if (output_file.empty()) {
        output_file = getOptionValue(args, "--output");
    }
    
    // Watch mode
    bool watch_mode = hasFlag(args, "-w") || hasFlag(args, "--watch");
    int interval = 2; // default 2 seconds
    
    std::string interval_str = getOptionValue(args, "-i");
    if (interval_str.empty()) {
        interval_str = getOptionValue(args, "--interval");
    }
    if (!interval_str.empty()) {
        try {
            interval = std::stoi(interval_str);
            if (interval < 1) interval = 1;
        } catch (...) {
            printError("Invalid interval: " + interval_str);
            return 1;
        }
    }
    
    // Main loop
    do {
        // Clear screen in watch mode (text format only)
        if (watch_mode && opts.format == "text") {
            std::cout << "\033[2J\033[H"; // Clear screen and move cursor to top
        }
        
        // Gather system information
        HardwareInfo hw;
        UtilizationInfo util;
        
        if (opts.show_static) {
            hw = getHardwareInfo();
        }
        
        if (opts.show_dynamic) {
            util = getUtilizationInfo();
        }
        
        // Format output
        std::string output = formatOutput(hw, util, opts);
        
        // Write to file or stdout
        if (!output_file.empty()) {
            std::ofstream file(output_file);
            if (!file) {
                printError("Cannot write to file: " + output_file);
                return 1;
            }
            file << output;
            file.close();
            
            if (!watch_mode) {
                std::cout << "Output written to: " << output_file << std::endl;
            }
        } else {
            std::cout << output;
        }
        
        // Sleep in watch mode
        if (watch_mode) {
            sleep(interval);
        }
        
    } while (watch_mode);
    
    return 0;
}
