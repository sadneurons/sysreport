#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include "system_info.h"

struct Config {
    // Display defaults
    bool use_colors = true;
    bool show_progress_bars = true;
    bool show_alerts = true;
    bool show_static = true;
    bool show_dynamic = true;
    bool show_timestamp = false;
    std::string default_format = "text"; // text, json, csv
    
    // Watch mode defaults
    int default_interval = 2;
    
    // Custom thresholds (percentages)
    double cpu_warning_threshold = 80.0;
    double cpu_critical_threshold = 90.0;
    double ram_warning_threshold = 80.0;
    double ram_critical_threshold = 90.0;
    double disk_warning_threshold = 80.0;
    double disk_critical_threshold = 90.0;
    double temp_warning_threshold = 60.0;
    double temp_critical_threshold = 80.0;
    
    // Filtering defaults
    bool cpu_only = false;
    bool memory_only = false;
    bool disk_only = false;
    bool network_only = false;
    bool process_only = false;
    
    // Process display
    int top_process_count = 5;
};

// Load configuration from file
Config loadConfig(const std::string& config_path = "");

// Get default config file path (~/.config/sysreport/config.conf)
std::string getDefaultConfigPath();

// Merge config with command-line options (CLI takes precedence)
void applyConfigToDisplayOptions(const Config& config, DisplayOptions& opts);

#endif
