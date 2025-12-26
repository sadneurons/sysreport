#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <string>
#include <deque>
#include <map>
#include <chrono>

struct MetricSnapshot {
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    double cpu_percent;
    double ram_percent;
    double swap_percent;
    std::vector<double> cpu_per_core;
    std::map<std::string, double> disk_usage;  // mount_point -> percent
    std::map<std::string, long> network_rx;    // interface -> bytes
    std::map<std::string, long> network_tx;
    double gpu_percent;
    double gpu_temp;
};

class MetricHistory {
private:
    std::deque<MetricSnapshot> snapshots;
    size_t max_history;
    MetricSnapshot baseline;
    bool has_baseline;
    
public:
    MetricHistory(size_t max_samples = 60);
    
    void addSnapshot(const MetricSnapshot& snapshot);
    void setBaseline(const MetricSnapshot& snapshot);
    void clearBaseline();
    bool hasBaseline() const;
    
    std::vector<double> getCpuHistory(size_t count = 20) const;
    std::vector<double> getRamHistory(size_t count = 20) const;
    std::vector<double> getGpuHistory(size_t count = 20) const;
    
    double getCpuTrend() const;
    double getRamTrend() const;
    double getGpuTrend() const;
    
    MetricSnapshot getBaseline() const;
    MetricSnapshot getLatest() const;
    
    std::string renderSparkline(const std::vector<double>& data) const;
    std::string getTrendArrow(double trend) const;
    
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
};

#endif
