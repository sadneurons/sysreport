#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>

// Trim whitespace from both ends
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Parse boolean value
static bool parseBool(const std::string& value) {
    std::string v = trim(value);
    return v == "true" || v == "yes" || v == "1" || v == "on";
}

// Parse integer value
static int parseInt(const std::string& value) {
    try {
        return std::stoi(trim(value));
    } catch (...) {
        return 0;
    }
}

// Parse double value
static double parseDouble(const std::string& value) {
    try {
        return std::stod(trim(value));
    } catch (...) {
        return 0.0;
    }
}

std::string getDefaultConfigPath() {
    const char* home = std::getenv("HOME");
    if (!home) return "";
    
    std::string config_dir = std::string(home) + "/.config/sysreport";
    return config_dir + "/config.conf";
}

Config loadConfig(const std::string& config_path) {
    Config config; // Start with defaults
    
    std::string path = config_path.empty() ? getDefaultConfigPath() : config_path;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return config; // Return defaults if file doesn't exist
    }
    
    std::string line;
    std::string current_section;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Section header
        if (line[0] == '[' && line[line.length() - 1] == ']') {
            current_section = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Key-value pair
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;
        
        std::string key = trim(line.substr(0, eq_pos));
        std::string value = trim(line.substr(eq_pos + 1));
        
        // Remove quotes if present
        if (value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        // Parse based on section and key
        if (current_section == "display" || current_section.empty()) {
            if (key == "colors") config.use_colors = parseBool(value);
            else if (key == "progress_bars") config.show_progress_bars = parseBool(value);
            else if (key == "alerts") config.show_alerts = parseBool(value);
            else if (key == "static_info") config.show_static = parseBool(value);
            else if (key == "dynamic_info") config.show_dynamic = parseBool(value);
            else if (key == "timestamp") config.show_timestamp = parseBool(value);
            else if (key == "format") config.default_format = value;
            else if (key == "top_processes") config.top_process_count = parseInt(value);
        }
        else if (current_section == "watch") {
            if (key == "default_interval") config.default_interval = parseInt(value);
        }
        else if (current_section == "thresholds") {
            if (key == "cpu_warning") config.cpu_warning_threshold = parseDouble(value);
            else if (key == "cpu_critical") config.cpu_critical_threshold = parseDouble(value);
            else if (key == "ram_warning") config.ram_warning_threshold = parseDouble(value);
            else if (key == "ram_critical") config.ram_critical_threshold = parseDouble(value);
            else if (key == "disk_warning") config.disk_warning_threshold = parseDouble(value);
            else if (key == "disk_critical") config.disk_critical_threshold = parseDouble(value);
            else if (key == "temp_warning") config.temp_warning_threshold = parseDouble(value);
            else if (key == "temp_critical") config.temp_critical_threshold = parseDouble(value);
        }
        else if (current_section == "filters") {
            if (key == "cpu_only") config.cpu_only = parseBool(value);
            else if (key == "memory_only") config.memory_only = parseBool(value);
            else if (key == "disk_only") config.disk_only = parseBool(value);
            else if (key == "network_only") config.network_only = parseBool(value);
            else if (key == "process_only") config.process_only = parseBool(value);
        }
    }
    
    return config;
}

void applyConfigToDisplayOptions(const Config& config, DisplayOptions& opts) {
    // Only apply config values that haven't been explicitly set by CLI
    // Note: In practice, we'd need to track which options were set via CLI
    // For now, we'll apply defaults and let main.cpp override with CLI args
    
    opts.use_colors = config.use_colors;
    opts.show_progress_bars = config.show_progress_bars;
    opts.show_alerts = config.show_alerts;
    opts.show_static = config.show_static;
    opts.show_dynamic = config.show_dynamic;
    opts.show_timestamp = config.show_timestamp;
    
    // Apply filters from config
    if (config.cpu_only) opts.cpu_only = true;
    if (config.memory_only) opts.memory_only = true;
    if (config.disk_only) opts.disk_only = true;
    if (config.network_only) opts.network_only = true;
    if (config.process_only) opts.process_only = true;
    
    // Initialize history-related fields
    opts.show_history = false;
    opts.show_baseline_comparison = false;
    opts.metric_history = nullptr;
}
