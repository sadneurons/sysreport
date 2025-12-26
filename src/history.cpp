#include "history.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

// Unicode sparkline characters
static const char* SPARK_CHARS[] = {"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};

MetricHistory::MetricHistory(size_t max_samples) 
    : max_history(max_samples), has_baseline(false) {
}

void MetricHistory::addSnapshot(const MetricSnapshot& snapshot) {
    snapshots.push_back(snapshot);
    
    // Keep only max_history samples
    while (snapshots.size() > max_history) {
        snapshots.pop_front();
    }
}

void MetricHistory::setBaseline(const MetricSnapshot& snapshot) {
    baseline = snapshot;
    has_baseline = true;
}

void MetricHistory::clearBaseline() {
    has_baseline = false;
}

bool MetricHistory::hasBaseline() const {
    return has_baseline;
}

std::vector<double> MetricHistory::getCpuHistory(size_t count) const {
    std::vector<double> history;
    size_t start = snapshots.size() > count ? snapshots.size() - count : 0;
    
    for (size_t i = start; i < snapshots.size(); i++) {
        history.push_back(snapshots[i].cpu_percent);
    }
    
    return history;
}

std::vector<double> MetricHistory::getRamHistory(size_t count) const {
    std::vector<double> history;
    size_t start = snapshots.size() > count ? snapshots.size() - count : 0;
    
    for (size_t i = start; i < snapshots.size(); i++) {
        history.push_back(snapshots[i].ram_percent);
    }
    
    return history;
}

std::vector<double> MetricHistory::getGpuHistory(size_t count) const {
    std::vector<double> history;
    size_t start = snapshots.size() > count ? snapshots.size() - count : 0;
    
    for (size_t i = start; i < snapshots.size(); i++) {
        history.push_back(snapshots[i].gpu_percent);
    }
    
    return history;
}

double MetricHistory::getCpuTrend() const {
    if (snapshots.size() < 2) return 0.0;
    
    // Simple linear regression over recent samples
    size_t count = std::min(snapshots.size(), (size_t)10);
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_xx = 0;
    
    for (size_t i = 0; i < count; i++) {
        size_t idx = snapshots.size() - count + i;
        double x = i;
        double y = snapshots[idx].cpu_percent;
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_xx += x * x;
    }
    
    double slope = (count * sum_xy - sum_x * sum_y) / (count * sum_xx - sum_x * sum_x);
    return slope;
}

double MetricHistory::getRamTrend() const {
    if (snapshots.size() < 2) return 0.0;
    
    size_t count = std::min(snapshots.size(), (size_t)10);
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_xx = 0;
    
    for (size_t i = 0; i < count; i++) {
        size_t idx = snapshots.size() - count + i;
        double x = i;
        double y = snapshots[idx].ram_percent;
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_xx += x * x;
    }
    
    double slope = (count * sum_xy - sum_x * sum_y) / (count * sum_xx - sum_x * sum_x);
    return slope;
}

double MetricHistory::getGpuTrend() const {
    if (snapshots.size() < 2) return 0.0;
    
    size_t count = std::min(snapshots.size(), (size_t)10);
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_xx = 0;
    
    for (size_t i = 0; i < count; i++) {
        size_t idx = snapshots.size() - count + i;
        double x = i;
        double y = snapshots[idx].gpu_percent;
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_xx += x * x;
    }
    
    double slope = (count * sum_xy - sum_x * sum_y) / (count * sum_xx - sum_x * sum_x);
    return slope;
}

MetricSnapshot MetricHistory::getBaseline() const {
    return baseline;
}

MetricSnapshot MetricHistory::getLatest() const {
    if (snapshots.empty()) {
        return MetricSnapshot();
    }
    return snapshots.back();
}

std::string MetricHistory::renderSparkline(const std::vector<double>& data) const {
    if (data.empty()) return "";
    
    // Find min and max
    double min_val = *std::min_element(data.begin(), data.end());
    double max_val = *std::max_element(data.begin(), data.end());
    
    // Handle case where all values are the same
    if (max_val - min_val < 0.01) {
        return std::string(data.size(), SPARK_CHARS[4][0]);
    }
    
    std::string sparkline;
    for (double val : data) {
        // Normalize to 0-7 range
        int idx = static_cast<int>((val - min_val) / (max_val - min_val) * 7);
        idx = std::max(0, std::min(7, idx));
        sparkline += SPARK_CHARS[idx];
    }
    
    return sparkline;
}

std::string MetricHistory::getTrendArrow(double trend) const {
    if (trend > 0.5) return " ↑";      // Rising
    if (trend < -0.5) return " ↓";     // Falling
    return " →";                        // Stable
}

bool MetricHistory::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file) return false;
    
    file << "timestamp,cpu_percent,ram_percent,swap_percent,gpu_percent,gpu_temp\n";
    
    for (const auto& snap : snapshots) {
        auto epoch = std::chrono::duration_cast<std::chrono::seconds>(
            snap.timestamp.time_since_epoch()).count();
        
        file << epoch << ","
             << snap.cpu_percent << ","
             << snap.ram_percent << ","
             << snap.swap_percent << ","
             << snap.gpu_percent << ","
             << snap.gpu_temp << "\n";
    }
    
    return true;
}

bool MetricHistory::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) return false;
    
    std::string line;
    std::getline(file, line); // Skip header
    
    snapshots.clear();
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        MetricSnapshot snap;
        
        long epoch;
        char comma;
        
        iss >> epoch >> comma
            >> snap.cpu_percent >> comma
            >> snap.ram_percent >> comma
            >> snap.swap_percent >> comma
            >> snap.gpu_percent >> comma
            >> snap.gpu_temp;
        
        snap.timestamp = std::chrono::steady_clock::time_point(
            std::chrono::seconds(epoch));
        
        snapshots.push_back(snap);
    }
    
    return true;
}
