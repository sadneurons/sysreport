#include "plugin.h"
#include "security.h"
#include "format.h"
#include <iostream>
#include <dlfcn.h>
#include <dirent.h>
#include <cstring>
#include <sstream>
#include <iomanip>

// ANSI color codes
const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_CYAN = "\033[36m";
const std::string COLOR_BOLD = "\033[1m";

PluginManager::PluginManager() 
    : plugin_dir("/usr/lib/sysreport/plugins")
    , security_manager(nullptr)
    , enforce_security(false) {
}

PluginManager::PluginManager(SecurityManager* sec_mgr)
    : plugin_dir("/usr/lib/sysreport/plugins")
    , security_manager(sec_mgr)
    , enforce_security(true) {
}

PluginManager::~PluginManager() {
    unloadAllPlugins();
}

bool PluginManager::loadPlugin(const std::string& plugin_path) {
    // Security validation
    if (security_manager && enforce_security) {
        if (!security_manager->validatePluginPath(plugin_path)) {
            std::cerr << "Security validation failed for: " << plugin_path << std::endl;
            return false;
        }
    }
    
    // Load shared library
    void* handle = dlopen(plugin_path.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load plugin: " << plugin_path << std::endl;
        std::cerr << "Error: " << dlerror() << std::endl;
        return false;
    }
    
    // Create plugin structure
    Plugin* plugin = new Plugin();
    plugin->handle = handle;
    plugin->path = plugin_path;
    plugin->initialized = false;
    plugin->security_validated = (security_manager != nullptr && enforce_security);
    
    // Load plugin info function
    plugin->get_info = (GetPluginInfoFunc)dlsym(handle, "get_plugin_info");
    if (!plugin->get_info) {
        std::cerr << "Plugin missing get_plugin_info(): " << plugin_path << std::endl;
        dlclose(handle);
        delete plugin;
        return false;
    }
    
    // Get plugin info
    plugin->info = plugin->get_info();
    
    // Load required functions
    plugin->get_metrics = (GetMetricsFunc)dlsym(handle, "get_metrics");
    if (!plugin->get_metrics) {
        std::cerr << "Plugin missing get_metrics(): " << plugin_path << std::endl;
        dlclose(handle);
        delete plugin;
        return false;
    }
    
    // Load optional functions
    plugin->init = (InitPluginFunc)dlsym(handle, "init_plugin");
    plugin->cleanup = (CleanupPluginFunc)dlsym(handle, "cleanup_plugin");
    
    // Validate plugin
    if (!validatePlugin(plugin)) {
        dlclose(handle);
        delete plugin;
        return false;
    }
    
    // Initialize plugin
    if (plugin->init) {
        if (!plugin->init()) {
            std::cerr << "Plugin initialization failed: " << plugin->info.name << std::endl;
            dlclose(handle);
            delete plugin;
            return false;
        }
    }
    plugin->initialized = true;
    
    // Add to loaded plugins
    loaded_plugins.push_back(plugin);
    
    std::cout << "Loaded plugin: " << plugin->info.name 
              << " v" << plugin->info.version << std::endl;
    
    return true;
}

bool PluginManager::loadPluginsFromDirectory(const std::string& directory) {
    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        std::cerr << "Cannot open plugin directory: " << directory << std::endl;
        return false;
    }
    
    plugin_dir = directory;
    int loaded_count = 0;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        
        // Only load .so files
        if (filename.length() > 3 && 
            filename.substr(filename.length() - 3) == ".so") {
            std::string full_path = directory + "/" + filename;
            if (loadPlugin(full_path)) {
                loaded_count++;
            }
        }
    }
    
    closedir(dir);
    
    std::cout << "Loaded " << loaded_count << " plugin(s) from " << directory << std::endl;
    return loaded_count > 0;
}

void PluginManager::unloadAllPlugins() {
    for (auto plugin : loaded_plugins) {
        unloadPlugin(plugin);
    }
    loaded_plugins.clear();
}

void PluginManager::unloadPlugin(Plugin* plugin) {
    if (!plugin) return;
    
    // Call cleanup if available
    if (plugin->cleanup && plugin->initialized) {
        plugin->cleanup();
    }
    
    // Unload library
    if (plugin->handle) {
        dlclose(plugin->handle);
    }
    
    delete plugin;
}

std::vector<Plugin*> PluginManager::getLoadedPlugins() const {
    return loaded_plugins;
}

Plugin* PluginManager::getPluginByName(const std::string& name) const {
    for (auto plugin : loaded_plugins) {
        if (plugin->info.name == name) {
            return plugin;
        }
    }
    return nullptr;
}

std::map<std::string, std::vector<MetricData>> PluginManager::collectAllMetrics() {
    std::map<std::string, std::vector<MetricData>> all_metrics;
    
    for (auto plugin : loaded_plugins) {
        if (plugin->initialized && plugin->get_metrics) {
            try {
                std::vector<MetricData> metrics = plugin->get_metrics();
                all_metrics[plugin->info.name] = metrics;
            } catch (const std::exception& e) {
                std::cerr << "Error collecting metrics from " << plugin->info.name 
                         << ": " << e.what() << std::endl;
            }
        }
    }
    
    return all_metrics;
}

std::vector<MetricData> PluginManager::collectMetricsFromPlugin(const std::string& plugin_name) {
    Plugin* plugin = getPluginByName(plugin_name);
    if (plugin && plugin->initialized && plugin->get_metrics) {
        try {
            return plugin->get_metrics();
        } catch (const std::exception& e) {
            std::cerr << "Error collecting metrics from " << plugin_name 
                     << ": " << e.what() << std::endl;
        }
    }
    return std::vector<MetricData>();
}

std::string PluginManager::formatPluginMetrics(bool use_colors) {
    std::ostringstream oss;
    
    auto all_metrics = collectAllMetrics();
    
    if (all_metrics.empty()) {
        return "";
    }
    
    oss << createSeparator(Icons::PLUGIN + " PLUGIN METRICS", 80) << "\n";
    
    for (const auto& pair : all_metrics) {
        const std::string& plugin_name = pair.first;
        const std::vector<MetricData>& metrics = pair.second;
        
        if (use_colors) {
            oss << COLOR_BOLD << COLOR_CYAN;
        }
        oss << "\n" << plugin_name << ":";
        if (use_colors) {
            oss << COLOR_RESET;
        }
        oss << "\n";
        
        for (const auto& metric : metrics) {
            oss << "  " << std::setw(20) << std::left << metric.name << ": ";
            if (use_colors) {
                oss << COLOR_GREEN;
            }
            oss << metric.value;
            if (!metric.unit.empty()) {
                oss << " " << metric.unit;
            }
            if (use_colors) {
                oss << COLOR_RESET;
            }
            
            if (!metric.description.empty()) {
                if (use_colors) {
                    oss << COLOR_YELLOW;
                }
                oss << "  (" << metric.description << ")";
                if (use_colors) {
                    oss << COLOR_RESET;
                }
            }
            oss << "\n";
        }
    }
    
    oss << "\n";
    return oss.str();
}

void PluginManager::listPlugins() const {
    if (loaded_plugins.empty()) {
        std::cout << "No plugins loaded." << std::endl;
        return;
    }
    
    std::cout << "\nLoaded Plugins:\n";
    std::cout << "===============\n\n";
    
    for (const auto& plugin : loaded_plugins) {
        std::cout << COLOR_BOLD << COLOR_CYAN << plugin->info.name 
                  << COLOR_RESET << " v" << plugin->info.version << "\n";
        std::cout << "  Author:      " << plugin->info.author << "\n";
        std::cout << "  Description: " << plugin->info.description << "\n";
        std::cout << "  Path:        " << plugin->path << "\n";
        std::cout << "  API Version: " << plugin->info.api_version << "\n";
        std::cout << "  Status:      " << (plugin->initialized ? 
                     COLOR_GREEN + "Initialized" : COLOR_YELLOW + "Not initialized") 
                  << COLOR_RESET << "\n";
        std::cout << "\n";
    }
}

bool PluginManager::validatePlugin(Plugin* plugin) {
    // Check API version
    if (plugin->info.api_version != SYSREPORT_PLUGIN_API_VERSION) {
        std::cerr << "Plugin API version mismatch: " << plugin->info.name 
                  << " (expected " << SYSREPORT_PLUGIN_API_VERSION 
                  << ", got " << plugin->info.api_version << ")" << std::endl;
        return false;
    }
    
    // Check required fields
    if (plugin->info.name.empty()) {
        std::cerr << "Plugin has empty name" << std::endl;
        return false;
    }
    
    return true;
}

// Security methods
void PluginManager::setSecurityManager(SecurityManager* sec_mgr) {
    security_manager = sec_mgr;
}

void PluginManager::setSecurityEnforcement(bool enforce) {
    enforce_security = enforce;
}
