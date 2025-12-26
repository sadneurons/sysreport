#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <string>
#include <vector>
#include <map>

// Process information
struct ProcessInfo {
    int pid;
    std::string name;
    double cpu_percent;
    long mem_mb;
};

// Disk partition information
struct DiskInfo {
    std::string mount_point;
    std::string device;
    long total_gb;
    long used_gb;
    long available_gb;
    double percent;
};

// Network interface information
struct NetworkInfo {
    std::string interface;
    long rx_bytes;
    long tx_bytes;
};

// Static hardware information
struct HardwareInfo {
    std::string cpu_model;
    long total_ram_mb;
    long total_swap_mb;
    std::vector<DiskInfo> disks;
    int cpu_cores;
    std::string os_info;
    std::vector<std::string> network_interfaces;
};

// Dynamic utilization information
struct UtilizationInfo {
    double cpu_percent;
    std::vector<double> cpu_per_core;
    long used_ram_mb;
    long available_ram_mb;
    double ram_percent;
    long used_swap_mb;
    long available_swap_mb;
    double swap_percent;
    std::vector<DiskInfo> disks;
    std::vector<NetworkInfo> network;
    std::vector<ProcessInfo> top_processes;
    std::string uptime;
    double load_avg_1;
    double load_avg_5;
    double load_avg_15;
    std::vector<double> temperatures;
};

// Display options
struct DisplayOptions {
    bool show_static;
    bool show_dynamic;
    bool use_colors;
    bool show_progress_bars;
    bool show_timestamp;
    bool show_alerts;
    std::string format; // text, json, csv
    
    // Filters
    bool cpu_only;
    bool memory_only;
    bool disk_only;
    bool network_only;
    bool process_only;
};

// Functions to gather system information
HardwareInfo getHardwareInfo();
UtilizationInfo getUtilizationInfo();

// Formatting functions
std::string formatOutput(const HardwareInfo& hw, const UtilizationInfo& util, const DisplayOptions& opts);
std::string getTimestamp();
std::string colorize(const std::string& text, const std::string& color);
std::string getProgressBar(double percent, int width = 20);
std::string getColorForPercent(double percent);

#endif // SYSTEM_INFO_H
