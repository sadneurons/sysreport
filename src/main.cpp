#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <chrono>
#include "cli.h"
#include "system_info.h"
#include "config.h"
#include "tui.h"
#include "history.h"
#include "exporters.h"
#include "daemon.h"
#include "plugin.h"
#include "security.h"

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
    
    // Check for root execution (security risk)
    if (SecurityManager::isRunningAsRoot() && !hasFlag(args, "--allow-root")) {
        std::cerr << "\n⚠️  WARNING: Running as root is a security risk!\n";
        std::cerr << "\nFor security reasons, sysreport should not be run as root.\n";
        std::cerr << "If you must run as root, use the --allow-root flag explicitly.\n";
        std::cerr << "\nExample: sudo sysreport --allow-root\n" << std::endl;
        return 1;
    }
    
    // Initialize security manager
    SecurityManager security_mgr;
    
    // Check signature verification setting
    bool no_verify_sigs = hasFlag(args, "--no-verify-signatures");
    if (no_verify_sigs) {
        security_mgr.requireSignatures(false);
        std::cerr << "⚠️  WARNING: Plugin signature verification disabled!\n" << std::endl;
    }
    
    // Check plugin security level
    std::string plugin_security = getOptionValue(args, "--plugin-security");
    bool no_sandbox = hasFlag(args, "--no-plugin-sandbox");
    
    // Initialize plugin manager with security (enabled by default)
    PluginManager plugin_manager;
    plugin_manager.setSecurityManager(&security_mgr);
    plugin_manager.setSecurityEnforcement(!no_sandbox);  // Default: true
    
    if (!no_sandbox && !plugin_security.empty()) {
        std::cout << "Plugin security level: " << plugin_security << std::endl;
    }
    
    // Handle plugin loading
    std::string plugin_file = getOptionValue(args, "--plugin");
    if (!plugin_file.empty()) {
        plugin_manager.loadPlugin(plugin_file);
    }
    
    std::string plugin_dir = getOptionValue(args, "--plugin-dir");
    if (!plugin_dir.empty()) {
        plugin_manager.loadPluginsFromDirectory(plugin_dir);
    }
    
    // Handle list plugins
    if (hasFlag(args, "--list-plugins")) {
        plugin_manager.listPlugins();
        return 0;
    }
    
    // Handle daemon mode
    if (hasFlag(args, "--daemon")) {
        DaemonConfig daemon_cfg;
        
        // Get daemon options
        std::string log_file = getOptionValue(args, "--daemon-log");
        if (!log_file.empty()) {
            daemon_cfg.log_file = log_file;
        }
        
        std::string interval_str = getOptionValue(args, "--daemon-interval");
        if (!interval_str.empty()) {
            try {
                daemon_cfg.interval_seconds = std::stoi(interval_str);
            } catch (...) {}
        }
        
        std::string webhook_url = getOptionValue(args, "--webhook");
        if (!webhook_url.empty()) {
            daemon_cfg.enable_webhooks = true;
            daemon_cfg.webhook_url = webhook_url;
        }
        
        // Determine export format
        if (hasFlag(args, "--prometheus")) {
            daemon_cfg.export_format = "prometheus";
        } else if (hasFlag(args, "--influxdb")) {
            daemon_cfg.export_format = "influxdb";
        } else if (hasFlag(args, "-f") || hasFlag(args, "--format")) {
            daemon_cfg.export_format = getOptionValue(args, "-f");
            if (daemon_cfg.export_format.empty()) {
                daemon_cfg.export_format = getOptionValue(args, "--format");
            }
        }
        
        // Daemonize process
        if (!DaemonMode::daemonize()) {
            std::cerr << "Failed to daemonize process" << std::endl;
            return 1;
        }
        
        // Write PID file
        DaemonMode::writePidFile(daemon_cfg.pid_file);
        
        // Start daemon
        DaemonMode daemon(daemon_cfg);
        daemon.start();
        
        // Remove PID file on exit
        DaemonMode::removePidFile(daemon_cfg.pid_file);
        
        return 0;
    }
    
    // Handle Prometheus export
    if (hasFlag(args, "--prometheus")) {
        UtilizationInfo util = getUtilizationInfo();
        std::cout << PrometheusExporter::exportMetrics(util);
        return 0;
    }
    
    // Handle InfluxDB export
    if (hasFlag(args, "--influxdb")) {
        UtilizationInfo util = getUtilizationInfo();
        std::cout << InfluxDBExporter::exportMetrics(util);
        return 0;
    }
    
    // Handle TUI mode
    if (hasFlag(args, "-T") || hasFlag(args, "--tui")) {
        TUI tui;
        
        // Get interval if specified
        std::string interval_str = getOptionValue(args, "-i");
        if (interval_str.empty()) {
            interval_str = getOptionValue(args, "--interval");
        }
        if (!interval_str.empty()) {
            try {
                int interval = std::stoi(interval_str);
                if (interval > 0) {
                    tui.setRefreshInterval(interval);
                }
            } catch (...) {}
        }
        
        tui.run();
        return 0;
    }
    
    // Load configuration file
    std::string config_path = getOptionValue(args, "--config");
    Config config = loadConfig(config_path);
    
    // Setup display options from config
    DisplayOptions opts;
    applyConfigToDisplayOptions(config, opts);
    
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
    
    // Display options - CLI overrides config
    if (hasFlag(args, "--no-color")) {
        opts.use_colors = false;
    } else if (hasFlag(args, "-c") || hasFlag(args, "--color")) {
        opts.use_colors = true;
    }
    // else: use config default (already set)
    
    if (hasFlag(args, "-p") || hasFlag(args, "--progress")) {
        opts.show_progress_bars = true;
    }
    
    if (hasFlag(args, "-t") || hasFlag(args, "--timestamp")) {
        opts.show_timestamp = true;
    }
    
    if (hasFlag(args, "--alerts")) {
        opts.show_alerts = true;
    }
    
    // History options
    opts.show_history = hasFlag(args, "--history");
    opts.show_baseline_comparison = hasFlag(args, "--baseline");
    
    std::string save_baseline_file = getOptionValue(args, "--save-baseline");
    std::string load_baseline_file = getOptionValue(args, "--load-baseline");
    
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
    int interval = config.default_interval; // use config default
    
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
    
    // Initialize history tracking
    MetricHistory history;
    
    // Load baseline if specified
    if (!load_baseline_file.empty()) {
        if (history.loadFromFile(load_baseline_file)) {
            auto latest = history.getLatest();
            if (latest.timestamp.time_since_epoch().count() > 0) {
                history.setBaseline(latest);
            } else {
                std::cerr << "Warning: Loaded baseline file but no data found\n";
            }
        } else {
            std::cerr << "Warning: Could not load baseline from " << load_baseline_file << "\n";
        }
    }
    
    // Main loop
    int iteration = 0;
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
        
        // Create snapshot for history
        if (opts.show_history || opts.show_baseline_comparison || !save_baseline_file.empty()) {
            MetricSnapshot snapshot;
            snapshot.timestamp = std::chrono::steady_clock::now();
            snapshot.cpu_percent = util.cpu_percent;
            snapshot.ram_percent = util.ram_percent;
            snapshot.swap_percent = util.swap_percent;
            
            // GPU if available
            if (!util.gpus.empty() && util.gpus[0].available) {
                snapshot.gpu_percent = util.gpus[0].utilization_percent;
                snapshot.gpu_temp = util.gpus[0].temperature;
            } else {
                snapshot.gpu_percent = 0.0;
                snapshot.gpu_temp = 0.0;
            }
            
            history.addSnapshot(snapshot);
            
            // Save baseline on first iteration if requested
            if (!save_baseline_file.empty() && iteration == 0) {
                history.setBaseline(snapshot);
            }
        }
        
        // Store history in opts for display
        opts.metric_history = &history;
        
        // Format output
        std::string output = formatOutput(hw, util, opts);
        
        // Add plugin metrics if any plugins are loaded
        if (plugin_manager.getLoadedPlugins().size() > 0) {
            output += plugin_manager.formatPluginMetrics(opts.use_colors);
        }
        
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
        
        iteration++;
        
    } while (watch_mode);
    
    // Save baseline if requested
    if (!save_baseline_file.empty()) {
        if (history.saveToFile(save_baseline_file)) {
            std::cout << "Baseline saved to: " << save_baseline_file << std::endl;
        } else {
            std::cerr << "Error: Could not save baseline to " << save_baseline_file << std::endl;
            return 1;
        }
    }
    
    return 0;
}
