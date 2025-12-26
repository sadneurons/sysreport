#include "../include/plugin.h"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/utsname.h>
#include <ctime>

// Plugin metadata
DECLARE_PLUGIN(
    "Custom System Info",
    "1.0.0",
    "Sysreport Team",
    "Provides custom system information metrics"
)

// Plugin initialization
DECLARE_PLUGIN_INIT() {
    return true;
}

// Plugin cleanup
DECLARE_PLUGIN_CLEANUP() {
}

// Get shell type
std::string getShell() {
    const char* shell = getenv("SHELL");
    if (shell) {
        std::string shell_str(shell);
        size_t pos = shell_str.find_last_of('/');
        if (pos != std::string::npos) {
            return shell_str.substr(pos + 1);
        }
        return shell_str;
    }
    return "unknown";
}

// Get terminal type
std::string getTerminal() {
    const char* term = getenv("TERM");
    return term ? term : "unknown";
}

// Get current user
std::string getCurrentUser() {
    const char* user = getenv("USER");
    return user ? user : "unknown";
}

// Get hostname
std::string getHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "unknown";
}

// Get kernel version
std::string getKernelVersion() {
    struct utsname info;
    if (uname(&info) == 0) {
        return std::string(info.release);
    }
    return "unknown";
}

// Count running processes
int getProcessCount() {
    int count = 0;
    std::ifstream stat("/proc/stat");
    std::string line;
    
    while (std::getline(stat, line)) {
        if (line.substr(0, 9) == "processes") {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> count;
            break;
        }
    }
    
    return count;
}

// Get current date/time
std::string getCurrentDateTime() {
    time_t now = time(nullptr);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buffer);
}

// Plugin metrics
DECLARE_PLUGIN_METRICS() {
    std::vector<MetricData> metrics;
    
    // User
    MetricData user;
    user.name = "User";
    user.value = getCurrentUser();
    user.description = "Current user";
    metrics.push_back(user);
    
    // Hostname
    MetricData host;
    host.name = "Hostname";
    host.value = getHostname();
    host.description = "System hostname";
    metrics.push_back(host);
    
    // Shell
    MetricData shell;
    shell.name = "Shell";
    shell.value = getShell();
    shell.description = "Current shell";
    metrics.push_back(shell);
    
    // Terminal
    MetricData term;
    term.name = "Terminal";
    term.value = getTerminal();
    term.description = "Terminal type";
    metrics.push_back(term);
    
    // Kernel
    MetricData kernel;
    kernel.name = "Kernel";
    kernel.value = getKernelVersion();
    kernel.description = "Linux kernel version";
    metrics.push_back(kernel);
    
    // Process count
    MetricData procs;
    procs.name = "Processes";
    procs.value = std::to_string(getProcessCount());
    procs.description = "Total processes created";
    metrics.push_back(procs);
    
    // Current time
    MetricData datetime;
    datetime.name = "Date/Time";
    datetime.value = getCurrentDateTime();
    datetime.description = "Current system time";
    metrics.push_back(datetime);
    
    return metrics;
}
