#include "system_info.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <thread>
#include <algorithm>
#include <dirent.h>

// ANSI color codes
const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_BLUE = "\033[34m";
const std::string COLOR_CYAN = "\033[36m";
const std::string COLOR_BOLD = "\033[1m";

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string getTimestamp() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

std::string colorize(const std::string& text, const std::string& color) {
    return color + text + COLOR_RESET;
}

std::string getColorForPercent(double percent) {
    if (percent >= 90) return COLOR_RED;
    if (percent >= 70) return COLOR_YELLOW;
    return COLOR_GREEN;
}

std::string getProgressBar(double percent, int width) {
    int filled = (int)(percent / 100.0 * width);
    std::string bar = "[";
    for (int i = 0; i < width; i++) {
        bar += (i < filled) ? "█" : "░";
    }
    bar += "]";
    return bar;
}

std::vector<DiskInfo> getDiskInfo() {
    std::vector<DiskInfo> disks;
    std::ifstream mounts("/proc/mounts");
    std::string line;
    
    while (std::getline(mounts, line)) {
        std::istringstream iss(line);
        std::string device, mount, type;
        iss >> device >> mount >> type;
        
        // Only process physical disks and common filesystems
        if (mount.find("/snap") == 0 || mount.find("/sys") == 0 || 
            mount.find("/proc") == 0 || mount.find("/dev") == 0 ||
            mount.find("/run") == 0) {
            continue;
        }
        
        if (type != "ext4" && type != "ext3" && type != "xfs" && 
            type != "btrfs" && type != "vfat" && type != "ntfs") {
            continue;
        }
        
        struct statvfs stat;
        if (statvfs(mount.c_str(), &stat) == 0) {
            DiskInfo disk;
            disk.mount_point = mount;
            disk.device = device;
            unsigned long total_bytes = stat.f_blocks * stat.f_frsize;
            unsigned long available_bytes = stat.f_bavail * stat.f_frsize;
            unsigned long used_bytes = total_bytes - available_bytes;
            
            disk.total_gb = total_bytes / (1024 * 1024 * 1024);
            disk.used_gb = used_bytes / (1024 * 1024 * 1024);
            disk.available_gb = available_bytes / (1024 * 1024 * 1024);
            disk.percent = total_bytes > 0 ? (double)used_bytes / total_bytes * 100.0 : 0.0;
            
            if (disk.total_gb > 0) {
                disks.push_back(disk);
            }
        }
    }
    
    return disks;
}

HardwareInfo getHardwareInfo() {
    HardwareInfo info;
    
    // Get CPU model
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                info.cpu_model = trim(line.substr(pos + 1));
                break;
            }
        }
    }
    
    // Get CPU cores
    info.cpu_cores = std::thread::hardware_concurrency();
    
    // Get total RAM and Swap
    std::ifstream meminfo("/proc/meminfo");
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            std::istringstream iss(line);
            std::string label;
            long kb;
            iss >> label >> kb;
            info.total_ram_mb = kb / 1024;
        } else if (line.find("SwapTotal:") != std::string::npos) {
            std::istringstream iss(line);
            std::string label;
            long kb;
            iss >> label >> kb;
            info.total_swap_mb = kb / 1024;
        }
    }
    
    // Get disk info
    info.disks = getDiskInfo();
    
    // Get OS info
    std::ifstream osrelease("/etc/os-release");
    while (std::getline(osrelease, line)) {
        if (line.find("PRETTY_NAME=") != std::string::npos) {
            size_t start = line.find("\"");
            size_t end = line.rfind("\"");
            if (start != std::string::npos && end != std::string::npos) {
                info.os_info = line.substr(start + 1, end - start - 1);
                break;
            }
        }
    }
    
    // Get network interfaces
    std::ifstream netdev("/proc/net/dev");
    std::getline(netdev, line); // skip header
    std::getline(netdev, line); // skip header
    while (std::getline(netdev, line)) {
        size_t colon = line.find(":");
        if (colon != std::string::npos) {
            std::string iface = trim(line.substr(0, colon));
            if (iface != "lo") {
                info.network_interfaces.push_back(iface);
            }
        }
    }
    
    return info;
}

std::vector<ProcessInfo> getTopProcesses(int count = 5) {
    std::vector<ProcessInfo> processes;
    DIR* dir = opendir("/proc");
    if (!dir) return processes;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_DIR) continue;
        
        // Check if directory name is a number (PID)
        std::string name = entry->d_name;
        if (name.find_first_not_of("0123456789") != std::string::npos) continue;
        
        int pid = std::stoi(name);
        std::string stat_path = "/proc/" + name + "/stat";
        std::string cmdline_path = "/proc/" + name + "/cmdline";
        
        std::ifstream stat_file(stat_path);
        if (!stat_file) continue;
        
        std::string stat_line;
        std::getline(stat_file, stat_line);
        
        // Parse process name and memory
        size_t start = stat_line.find('(');
        size_t end = stat_line.rfind(')');
        if (start == std::string::npos || end == std::string::npos) continue;
        
        std::string proc_name = stat_line.substr(start + 1, end - start - 1);
        
        // Get memory from statm
        std::string statm_path = "/proc/" + name + "/statm";
        std::ifstream statm_file(statm_path);
        long pages;
        if (statm_file >> pages) {
            ProcessInfo proc;
            proc.pid = pid;
            proc.name = proc_name;
            proc.mem_mb = pages * 4 / 1024; // Assuming 4KB pages
            proc.cpu_percent = 0.0; // Simplified - would need time delta for accurate CPU%
            
            if (proc.mem_mb > 0) {
                processes.push_back(proc);
            }
        }
    }
    closedir(dir);
    
    // Sort by memory and take top N
    std::sort(processes.begin(), processes.end(), 
              [](const ProcessInfo& a, const ProcessInfo& b) { return a.mem_mb > b.mem_mb; });
    
    if (processes.size() > (size_t)count) {
        processes.resize(count);
    }
    
    return processes;
}

std::vector<double> getPerCoreUsage() {
    std::vector<double> usage;
    std::ifstream stat1("/proc/stat");
    std::string line;
    
    // Skip first line (overall CPU)
    std::getline(stat1, line);
    
    std::vector<std::vector<long>> cpu_times1;
    while (std::getline(stat1, line)) {
        if (line.substr(0, 3) != "cpu") break;
        
        std::istringstream iss(line);
        std::string cpu;
        long user, nice, system, idle;
        iss >> cpu >> user >> nice >> system >> idle;
        cpu_times1.push_back({user, nice, system, idle});
    }
    stat1.close();
    
    usleep(100000); // 100ms
    
    std::ifstream stat2("/proc/stat");
    std::getline(stat2, line);
    
    std::vector<std::vector<long>> cpu_times2;
    while (std::getline(stat2, line)) {
        if (line.substr(0, 3) != "cpu") break;
        
        std::istringstream iss(line);
        std::string cpu;
        long user, nice, system, idle;
        iss >> cpu >> user >> nice >> system >> idle;
        cpu_times2.push_back({user, nice, system, idle});
    }
    stat2.close();
    
    for (size_t i = 0; i < cpu_times1.size() && i < cpu_times2.size(); i++) {
        long total1 = cpu_times1[i][0] + cpu_times1[i][1] + cpu_times1[i][2] + cpu_times1[i][3];
        long total2 = cpu_times2[i][0] + cpu_times2[i][1] + cpu_times2[i][2] + cpu_times2[i][3];
        long idle_delta = cpu_times2[i][3] - cpu_times1[i][3];
        long total_delta = total2 - total1;
        
        double percent = total_delta > 0 ? (1.0 - (double)idle_delta / total_delta) * 100.0 : 0.0;
        usage.push_back(percent);
    }
    
    return usage;
}

std::vector<double> getTemperatures() {
    std::vector<double> temps;
    
    // Try to read from thermal zones
    for (int i = 0; i < 10; i++) {
        std::string path = "/sys/class/thermal/thermal_zone" + std::to_string(i) + "/temp";
        std::ifstream temp_file(path);
        if (temp_file) {
            long temp_millidegrees;
            temp_file >> temp_millidegrees;
            temps.push_back(temp_millidegrees / 1000.0);
        }
    }
    
    return temps;
}

UtilizationInfo getUtilizationInfo() {
    UtilizationInfo info;
    
    // Get RAM usage
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long mem_total = 0, mem_available = 0, swap_total = 0, swap_free = 0;
    
    while (std::getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string label;
        long kb;
        
        if (line.find("MemTotal:") != std::string::npos) {
            iss >> label >> kb;
            mem_total = kb / 1024;
        } else if (line.find("MemAvailable:") != std::string::npos) {
            iss >> label >> kb;
            mem_available = kb / 1024;
        } else if (line.find("SwapTotal:") != std::string::npos) {
            iss >> label >> kb;
            swap_total = kb / 1024;
        } else if (line.find("SwapFree:") != std::string::npos) {
            iss >> label >> kb;
            swap_free = kb / 1024;
        }
    }
    
    info.available_ram_mb = mem_available;
    info.used_ram_mb = mem_total - mem_available;
    info.ram_percent = mem_total > 0 ? (double)(mem_total - mem_available) / mem_total * 100.0 : 0.0;
    
    info.available_swap_mb = swap_free;
    info.used_swap_mb = swap_total - swap_free;
    info.swap_percent = swap_total > 0 ? (double)(swap_total - swap_free) / swap_total * 100.0 : 0.0;
    
    // Get disk usage
    info.disks = getDiskInfo();
    
    // Get CPU usage (overall)
    std::ifstream stat1("/proc/stat");
    std::getline(stat1, line);
    long user1, nice1, system1, idle1;
    sscanf(line.c_str(), "cpu %ld %ld %ld %ld", &user1, &nice1, &system1, &idle1);
    stat1.close();
    
    usleep(100000); // 100ms
    
    std::ifstream stat2("/proc/stat");
    std::getline(stat2, line);
    long user2, nice2, system2, idle2;
    sscanf(line.c_str(), "cpu %ld %ld %ld %ld", &user2, &nice2, &system2, &idle2);
    stat2.close();
    
    long total1 = user1 + nice1 + system1 + idle1;
    long total2 = user2 + nice2 + system2 + idle2;
    long idle_delta = idle2 - idle1;
    long total_delta = total2 - total1;
    
    info.cpu_percent = total_delta > 0 ? (1.0 - (double)idle_delta / total_delta) * 100.0 : 0.0;
    
    // Get per-core usage
    info.cpu_per_core = getPerCoreUsage();
    
    // Get uptime
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        long days = si.uptime / 86400;
        long hours = (si.uptime % 86400) / 3600;
        long minutes = (si.uptime % 3600) / 60;
        
        std::ostringstream oss;
        if (days > 0) oss << days << "d ";
        if (hours > 0 || days > 0) oss << hours << "h ";
        oss << minutes << "m";
        info.uptime = oss.str();
        
        info.load_avg_1 = si.loads[0] / 65536.0;
        info.load_avg_5 = si.loads[1] / 65536.0;
        info.load_avg_15 = si.loads[2] / 65536.0;
    }
    
    // Get network stats
    std::ifstream netdev("/proc/net/dev");
    std::getline(netdev, line); // skip headers
    std::getline(netdev, line);
    
    while (std::getline(netdev, line)) {
        size_t colon = line.find(":");
        if (colon != std::string::npos) {
            std::string iface = trim(line.substr(0, colon));
            if (iface == "lo") continue;
            
            std::istringstream iss(line.substr(colon + 1));
            NetworkInfo net;
            net.interface = iface;
            iss >> net.rx_bytes >> std::ws;
            for (int i = 0; i < 7; i++) {
                long dummy;
                iss >> dummy;
            }
            iss >> net.tx_bytes;
            
            info.network.push_back(net);
        }
    }
    
    // Get top processes
    info.top_processes = getTopProcesses(5);
    
    // Get temperatures
    info.temperatures = getTemperatures();
    
    return info;
}

std::string formatAsText(const HardwareInfo& hw, const UtilizationInfo& util, const DisplayOptions& opts) {
    std::ostringstream oss;
    
    if (opts.show_timestamp) {
        oss << colorize("Timestamp: " + getTimestamp(), COLOR_CYAN) << "\n\n";
    }
    
    if (opts.show_static && !opts.cpu_only && !opts.memory_only && !opts.disk_only && !opts.network_only && !opts.process_only) {
        oss << colorize("=== HARDWARE INFORMATION ===", COLOR_BOLD) << "\n";
        oss << "OS:         " << hw.os_info << "\n";
        oss << "CPU Model:  " << hw.cpu_model << "\n";
        oss << "CPU Cores:  " << hw.cpu_cores << "\n";
        oss << "Total RAM:  " << hw.total_ram_mb << " MB\n";
        if (hw.total_swap_mb > 0) {
            oss << "Total Swap: " << hw.total_swap_mb << " MB\n";
        }
        oss << "Disks:      " << hw.disks.size() << " partition(s)\n";
        for (const auto& disk : hw.disks) {
            oss << "  - " << disk.mount_point << " (" << disk.device << "): " << disk.total_gb << " GB\n";
        }
        if (!hw.network_interfaces.empty()) {
            oss << "Network:    ";
            for (size_t i = 0; i < hw.network_interfaces.size(); i++) {
                if (i > 0) oss << ", ";
                oss << hw.network_interfaces[i];
            }
            oss << "\n";
        }
        oss << "\n";
    }
    
    if (opts.show_dynamic) {
        oss << colorize("=== UTILIZATION ===", COLOR_BOLD) << "\n";
        
        if (!opts.memory_only && !opts.disk_only && !opts.network_only && !opts.process_only) {
            std::string color = getColorForPercent(util.cpu_percent);
            oss << "CPU Usage:  ";
            if (opts.use_colors) oss << color;
            oss << std::fixed << std::setprecision(1) << util.cpu_percent << "%";
            if (opts.use_colors) oss << COLOR_RESET;
            if (opts.show_progress_bars) {
                oss << " " << getProgressBar(util.cpu_percent);
            }
            oss << "\n";
            
            if (opts.show_alerts && util.cpu_percent >= 90) {
                oss << colorize("  ⚠ WARNING: High CPU usage!", COLOR_RED) << "\n";
            }
            
            // Per-core usage
            if (!util.cpu_per_core.empty() && !opts.cpu_only) {
                oss << "Per-Core:   ";
                for (size_t i = 0; i < util.cpu_per_core.size(); i++) {
                    if (i > 0 && i % 8 == 0) oss << "\n            ";
                    std::string core_color = getColorForPercent(util.cpu_per_core[i]);
                    if (opts.use_colors) oss << core_color;
                    oss << std::setw(4) << std::fixed << std::setprecision(0) << util.cpu_per_core[i] << "%";
                    if (opts.use_colors) oss << COLOR_RESET;
                    if (i < util.cpu_per_core.size() - 1) oss << " ";
                }
                oss << "\n";
            }
            
            oss << "Load Avg:   " << std::fixed << std::setprecision(2) 
                << util.load_avg_1 << ", " << util.load_avg_5 << ", " << util.load_avg_15 << "\n";
            oss << "Uptime:     " << util.uptime << "\n";
            
            if (!util.temperatures.empty()) {
                oss << "Temp:       ";
                for (size_t i = 0; i < util.temperatures.size(); i++) {
                    if (i > 0) oss << ", ";
                    double temp = util.temperatures[i];
                    std::string temp_color = temp >= 80 ? COLOR_RED : (temp >= 60 ? COLOR_YELLOW : COLOR_GREEN);
                    if (opts.use_colors) oss << temp_color;
                    oss << std::fixed << std::setprecision(1) << temp << "°C";
                    if (opts.use_colors) oss << COLOR_RESET;
                }
                oss << "\n";
            }
            oss << "\n";
        }
        
        if (!opts.cpu_only && !opts.disk_only && !opts.network_only && !opts.process_only) {
            std::string ram_color = getColorForPercent(util.ram_percent);
            oss << "RAM Used:   ";
            if (opts.use_colors) oss << ram_color;
            oss << util.used_ram_mb << " MB / " << std::fixed << std::setprecision(1) << util.ram_percent << "%";
            if (opts.use_colors) oss << COLOR_RESET;
            if (opts.show_progress_bars) {
                oss << " " << getProgressBar(util.ram_percent);
            }
            oss << "\n";
            oss << "RAM Avail:  " << util.available_ram_mb << " MB\n";
            
            if (opts.show_alerts && util.ram_percent >= 90) {
                oss << colorize("  ⚠ WARNING: Low memory!", COLOR_RED) << "\n";
            }
            
            if (util.used_swap_mb > 0) {
                std::string swap_color = getColorForPercent(util.swap_percent);
                oss << "Swap Used:  ";
                if (opts.use_colors) oss << swap_color;
                oss << util.used_swap_mb << " MB / " << std::fixed << std::setprecision(1) << util.swap_percent << "%";
                if (opts.use_colors) oss << COLOR_RESET;
                if (opts.show_progress_bars) {
                    oss << " " << getProgressBar(util.swap_percent);
                }
                oss << "\n";
            }
            oss << "\n";
        }
        
        if (!opts.cpu_only && !opts.memory_only && !opts.network_only && !opts.process_only) {
            for (const auto& disk : util.disks) {
                std::string disk_color = getColorForPercent(disk.percent);
                oss << "Disk " << disk.mount_point << ":\n";
                oss << "  Used:     ";
                if (opts.use_colors) oss << disk_color;
                oss << disk.used_gb << " GB / " << std::fixed << std::setprecision(1) << disk.percent << "%";
                if (opts.use_colors) oss << COLOR_RESET;
                if (opts.show_progress_bars) {
                    oss << " " << getProgressBar(disk.percent);
                }
                oss << "\n";
                oss << "  Available: " << disk.available_gb << " GB\n";
                
                if (opts.show_alerts && disk.percent >= 90) {
                    oss << colorize("  ⚠ WARNING: Low disk space!", COLOR_RED) << "\n";
                }
            }
            oss << "\n";
        }
        
        if (!opts.cpu_only && !opts.memory_only && !opts.disk_only && !opts.process_only) {
            if (!util.network.empty()) {
                oss << "Network:\n";
                for (const auto& net : util.network) {
                    oss << "  " << std::setw(10) << std::left << net.interface << ": ";
                    oss << "RX " << std::setw(10) << std::right << (net.rx_bytes / 1024 / 1024) << " MB  ";
                    oss << "TX " << std::setw(10) << (net.tx_bytes / 1024 / 1024) << " MB\n";
                }
                oss << "\n";
            }
        }
        
        if (!opts.cpu_only && !opts.memory_only && !opts.disk_only && !opts.network_only) {
            if (!util.top_processes.empty()) {
                oss << "Top Processes (by memory):\n";
                for (const auto& proc : util.top_processes) {
                    oss << "  " << std::setw(7) << proc.pid << "  ";
                    oss << std::setw(30) << std::left << proc.name.substr(0, 30) << "  ";
                    oss << std::setw(8) << std::right << proc.mem_mb << " MB\n";
                }
            }
        }
    }
    
    return oss.str();
}

std::string formatAsJson(const HardwareInfo& hw, const UtilizationInfo& util, const DisplayOptions& opts) {
    std::ostringstream oss;
    oss << "{\n";
    
    if (opts.show_timestamp) {
        oss << "  \"timestamp\": \"" << getTimestamp() << "\",\n";
    }
    
    if (opts.show_static) {
        oss << "  \"hardware\": {\n";
        oss << "    \"os\": \"" << hw.os_info << "\",\n";
        oss << "    \"cpu_model\": \"" << hw.cpu_model << "\",\n";
        oss << "    \"cpu_cores\": " << hw.cpu_cores << ",\n";
        oss << "    \"total_ram_mb\": " << hw.total_ram_mb << ",\n";
        oss << "    \"total_swap_mb\": " << hw.total_swap_mb << ",\n";
        oss << "    \"disks\": [\n";
        for (size_t i = 0; i < hw.disks.size(); i++) {
            oss << "      {\"mount\": \"" << hw.disks[i].mount_point << "\", ";
            oss << "\"device\": \"" << hw.disks[i].device << "\", ";
            oss << "\"total_gb\": " << hw.disks[i].total_gb << "}";
            if (i < hw.disks.size() - 1) oss << ",";
            oss << "\n";
        }
        oss << "    ]\n";
        oss << "  }";
        if (opts.show_dynamic) oss << ",";
        oss << "\n";
    }
    
    if (opts.show_dynamic) {
        oss << "  \"utilization\": {\n";
        oss << "    \"cpu_percent\": " << std::fixed << std::setprecision(1) << util.cpu_percent << ",\n";
        oss << "    \"load_avg\": [" << util.load_avg_1 << ", " << util.load_avg_5 << ", " << util.load_avg_15 << "],\n";
        oss << "    \"uptime\": \"" << util.uptime << "\",\n";
        oss << "    \"used_ram_mb\": " << util.used_ram_mb << ",\n";
        oss << "    \"available_ram_mb\": " << util.available_ram_mb << ",\n";
        oss << "    \"ram_percent\": " << util.ram_percent << ",\n";
        oss << "    \"used_swap_mb\": " << util.used_swap_mb << ",\n";
        oss << "    \"swap_percent\": " << util.swap_percent << ",\n";
        oss << "    \"disks\": [\n";
        for (size_t i = 0; i < util.disks.size(); i++) {
            oss << "      {\"mount\": \"" << util.disks[i].mount_point << "\", ";
            oss << "\"used_gb\": " << util.disks[i].used_gb << ", ";
            oss << "\"available_gb\": " << util.disks[i].available_gb << ", ";
            oss << "\"percent\": " << util.disks[i].percent << "}";
            if (i < util.disks.size() - 1) oss << ",";
            oss << "\n";
        }
        oss << "    ]\n";
        oss << "  }\n";
    }
    
    oss << "}\n";
    return oss.str();
}

std::string formatAsCsv(const HardwareInfo& hw, const UtilizationInfo& util, const DisplayOptions& opts) {
    std::ostringstream oss;
    
    if (opts.show_timestamp) {
        oss << "Timestamp," << getTimestamp() << "\n\n";
    }
    
    oss << "Category,Metric,Value,Unit\n";
    
    if (opts.show_static) {
        oss << "Hardware,OS," << hw.os_info << ",\n";
        oss << "Hardware,CPU Model," << hw.cpu_model << ",\n";
        oss << "Hardware,CPU Cores," << hw.cpu_cores << ",count\n";
        oss << "Hardware,Total RAM," << hw.total_ram_mb << ",MB\n";
        oss << "Hardware,Total Swap," << hw.total_swap_mb << ",MB\n";
    }
    
    if (opts.show_dynamic) {
        oss << "Utilization,CPU Usage," << std::fixed << std::setprecision(1) << util.cpu_percent << ",%\n";
        oss << "Utilization,Load Avg 1min," << util.load_avg_1 << ",\n";
        oss << "Utilization,Load Avg 5min," << util.load_avg_5 << ",\n";
        oss << "Utilization,Load Avg 15min," << util.load_avg_15 << ",\n";
        oss << "Utilization,Uptime," << util.uptime << ",\n";
        oss << "Utilization,RAM Used," << util.used_ram_mb << ",MB\n";
        oss << "Utilization,RAM Available," << util.available_ram_mb << ",MB\n";
        oss << "Utilization,RAM Usage," << util.ram_percent << ",%\n";
        if (util.used_swap_mb > 0) {
            oss << "Utilization,Swap Used," << util.used_swap_mb << ",MB\n";
            oss << "Utilization,Swap Usage," << util.swap_percent << ",%\n";
        }
        for (const auto& disk : util.disks) {
            oss << "Disk " << disk.mount_point << ",Used," << disk.used_gb << ",GB\n";
            oss << "Disk " << disk.mount_point << ",Available," << disk.available_gb << ",GB\n";
            oss << "Disk " << disk.mount_point << ",Usage," << disk.percent << ",%\n";
        }
    }
    
    return oss.str();
}

std::string formatOutput(const HardwareInfo& hw, const UtilizationInfo& util, const DisplayOptions& opts) {
    if (opts.format == "json") {
        return formatAsJson(hw, util, opts);
    } else if (opts.format == "csv") {
        return formatAsCsv(hw, util, opts);
    } else {
        return formatAsText(hw, util, opts);
    }
}
