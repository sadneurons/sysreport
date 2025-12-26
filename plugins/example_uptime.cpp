#include "../include/plugin.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/sysinfo.h>

// Plugin metadata
DECLARE_PLUGIN("System Uptime", "1.0.0", "Sysreport Team", "Provides detailed system uptime information")

// Plugin initialization
DECLARE_PLUGIN_INIT() {
    // No initialization needed for this plugin
    return true;
}

// Plugin cleanup
DECLARE_PLUGIN_CLEANUP() {
    // No cleanup needed
}

// Format seconds to readable string
std::string formatUptime(long seconds) {
    int days = seconds / (24 * 3600);
    seconds %= (24 * 3600);
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    
    std::ostringstream oss;
    if (days > 0) {
        oss << days << "d " << hours << "h " << minutes << "m";
    } else if (hours > 0) {
        oss << hours << "h " << minutes << "m";
    } else {
        oss << minutes << "m";
    }
    
    return oss.str();
}

// Get boot time
std::string getBootTime() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        return "unknown";
    }
    
    time_t boot_time = time(nullptr) - info.uptime;
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&boot_time));
    
    return std::string(buffer);
}

// Plugin metrics
DECLARE_PLUGIN_METRICS() {
    std::vector<MetricData> metrics;
    
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        return metrics;
    }
    
    // Total uptime
    MetricData uptime;
    uptime.name = "Uptime";
    uptime.value = formatUptime(info.uptime);
    uptime.unit = "";
    uptime.description = "Total system uptime";
    metrics.push_back(uptime);
    
    // Uptime in seconds
    MetricData uptime_sec;
    uptime_sec.name = "Uptime (seconds)";
    uptime_sec.value = std::to_string(info.uptime);
    uptime_sec.unit = "s";
    uptime_sec.description = "Raw uptime in seconds";
    metrics.push_back(uptime_sec);
    
    // Boot time
    MetricData boot;
    boot.name = "Boot Time";
    boot.value = getBootTime();
    boot.unit = "";
    boot.description = "When the system was last booted";
    metrics.push_back(boot);
    
    // Idle time (total across all CPUs)
    MetricData idle;
    idle.name = "Idle Time";
    idle.value = formatUptime(info.uptime - (info.loads[0] / 65536));
    idle.unit = "";
    idle.description = "Approximate total idle time";
    metrics.push_back(idle);
    
    return metrics;
}
