#ifndef DAEMON_H
#define DAEMON_H

#include "system_info.h"
#include "config.h"
#include <string>
#include <fstream>

struct DaemonConfig {
    int interval_seconds;
    std::string log_file;
    std::string pid_file;
    std::string export_format; // "prometheus", "influxdb", "json", "csv"
    bool enable_webhooks;
    std::string webhook_url;
    
    // Alert thresholds
    double cpu_threshold;
    double memory_threshold;
    double disk_threshold;
    double gpu_threshold;
    
    DaemonConfig();
};

class DaemonMode {
private:
    DaemonConfig config;
    std::ofstream log_stream;
    bool running;
    
public:
    DaemonMode(const DaemonConfig& cfg);
    ~DaemonMode();
    
    bool start();
    void stop();
    bool isRunning() const;
    
    static bool daemonize();
    static bool writePidFile(const std::string& pid_file);
    static bool removePidFile(const std::string& pid_file);
    
private:
    void run();
    void checkAlerts(const UtilizationInfo& util);
    void logMetrics(const UtilizationInfo& util);
    std::string formatLogEntry(const UtilizationInfo& util);
};

#endif // DAEMON_H
