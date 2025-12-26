#include "daemon.h"
#include "exporters.h"
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctime>
#include <sstream>
#include <iomanip>

DaemonConfig::DaemonConfig() 
    : interval_seconds(60),
      log_file("/var/log/sysreport.log"),
      pid_file("/var/run/sysreport.pid"),
      export_format("json"),
      enable_webhooks(false),
      cpu_threshold(90.0),
      memory_threshold(90.0),
      disk_threshold(90.0),
      gpu_threshold(90.0) {
}

DaemonMode::DaemonMode(const DaemonConfig& cfg) 
    : config(cfg), running(false) {
}

DaemonMode::~DaemonMode() {
    stop();
}

bool DaemonMode::start() {
    if (running) return false;
    
    // Open log file
    log_stream.open(config.log_file, std::ios::app);
    if (!log_stream) {
        std::cerr << "Failed to open log file: " << config.log_file << std::endl;
        return false;
    }
    
    running = true;
    
    // Log startup
    time_t now = time(nullptr);
    log_stream << "=== Sysreport daemon started at " << ctime(&now);
    log_stream << "Export format: " << config.export_format << std::endl;
    log_stream << "Interval: " << config.interval_seconds << " seconds" << std::endl;
    log_stream.flush();
    
    // Run monitoring loop
    run();
    
    return true;
}

void DaemonMode::stop() {
    if (!running) return;
    
    running = false;
    
    if (log_stream.is_open()) {
        time_t now = time(nullptr);
        log_stream << "=== Sysreport daemon stopped at " << ctime(&now);
        log_stream.close();
    }
}

bool DaemonMode::isRunning() const {
    return running;
}

void DaemonMode::run() {
    while (running) {
        // Gather metrics
        UtilizationInfo util = getUtilizationInfo();
        
        // Check for alerts
        if (config.enable_webhooks) {
            checkAlerts(util);
        }
        
        // Log metrics
        logMetrics(util);
        
        // Sleep for interval
        sleep(config.interval_seconds);
    }
}

void DaemonMode::checkAlerts(const UtilizationInfo& util) {
    WebhookClient webhook(config.webhook_url);
    
    // CPU alert
    if (util.cpu_percent >= config.cpu_threshold) {
        webhook.sendAlert("cpu_usage", util.cpu_percent, config.cpu_threshold, "warning");
    }
    
    // Memory alert
    if (util.ram_percent >= config.memory_threshold) {
        webhook.sendAlert("memory_usage", util.ram_percent, config.memory_threshold, "warning");
    }
    
    // Disk alerts
    for (const auto& disk : util.disks) {
        if (disk.percent >= config.disk_threshold) {
            webhook.sendAlert("disk_usage_" + disk.mount_point, disk.percent, 
                            config.disk_threshold, "warning");
        }
    }
    
    // GPU alert
    if (!util.gpus.empty() && util.gpus[0].available) {
        if (util.gpus[0].utilization_percent >= config.gpu_threshold) {
            webhook.sendAlert("gpu_usage", util.gpus[0].utilization_percent,
                            config.gpu_threshold, "info");
        }
    }
}

void DaemonMode::logMetrics(const UtilizationInfo& util) {
    std::string entry = formatLogEntry(util);
    log_stream << entry << std::endl;
    log_stream.flush();
}

std::string DaemonMode::formatLogEntry(const UtilizationInfo& util) {
    if (config.export_format == "prometheus") {
        return PrometheusExporter::exportMetrics(util);
    } else if (config.export_format == "influxdb") {
        return InfluxDBExporter::exportMetrics(util);
    } else if (config.export_format == "csv") {
        std::ostringstream oss;
        time_t now = time(nullptr);
        oss << now << ","
            << std::fixed << std::setprecision(2)
            << util.cpu_percent << ","
            << util.ram_percent << ","
            << util.swap_percent << ",";
        
        if (!util.gpus.empty() && util.gpus[0].available) {
            oss << util.gpus[0].utilization_percent << ","
                << util.gpus[0].temperature;
        } else {
            oss << "0,0";
        }
        
        return oss.str();
    } else {
        // JSON format
        std::ostringstream oss;
        time_t now = time(nullptr);
        oss << "{\"timestamp\":" << now
            << ",\"cpu\":" << std::fixed << std::setprecision(2) << util.cpu_percent
            << ",\"memory\":" << util.ram_percent
            << ",\"swap\":" << util.swap_percent;
        
        if (!util.gpus.empty() && util.gpus[0].available) {
            oss << ",\"gpu\":" << util.gpus[0].utilization_percent
                << ",\"gpu_temp\":" << util.gpus[0].temperature;
        }
        
        oss << "}";
        return oss.str();
    }
}

bool DaemonMode::daemonize() {
    // Fork parent process
    pid_t pid = fork();
    
    if (pid < 0) {
        return false;
    }
    
    // Exit parent process
    if (pid > 0) {
        exit(0);
    }
    
    // Create new session
    if (setsid() < 0) {
        return false;
    }
    
    // Fork again to ensure we're not session leader
    pid = fork();
    
    if (pid < 0) {
        return false;
    }
    
    if (pid > 0) {
        exit(0);
    }
    
    // Change working directory
    chdir("/");
    
    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Set file permissions
    umask(0);
    
    return true;
}

bool DaemonMode::writePidFile(const std::string& pid_file) {
    std::ofstream file(pid_file);
    if (!file) return false;
    
    file << getpid() << std::endl;
    return true;
}

bool DaemonMode::removePidFile(const std::string& pid_file) {
    return unlink(pid_file.c_str()) == 0;
}
