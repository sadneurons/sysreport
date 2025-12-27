#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>
#include <vector>
#include <map>
#include <functional>

// Forward declarations
class SecurityManager;

// Plugin API version
#define SYSREPORT_PLUGIN_API_VERSION 1

// Forward declarations
struct PluginInfo;
struct MetricData;

// Plugin metric data structure
struct MetricData {
    std::string name;
    std::string value;
    std::string unit;
    std::string description;
};

// Plugin information
struct PluginInfo {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    int api_version;
};

// Plugin interface - plugins must implement these functions
typedef PluginInfo (*GetPluginInfoFunc)();
typedef std::vector<MetricData> (*GetMetricsFunc)();
typedef bool (*InitPluginFunc)();
typedef void (*CleanupPluginFunc)();

// Plugin handle structure
struct Plugin {
    void* handle;              // dlopen handle
    std::string path;          // Plugin file path
    PluginInfo info;           // Plugin metadata
    
    // Function pointers
    GetPluginInfoFunc get_info;
    GetMetricsFunc get_metrics;
    InitPluginFunc init;
    CleanupPluginFunc cleanup;
    
    bool initialized;
    bool security_validated;
};

// Forward declaration for SecurityManager
class SecurityManager;

// Plugin Manager
class PluginManager {
private:
    std::vector<Plugin*> loaded_plugins;
    std::string plugin_dir;
    SecurityManager* security_manager;
    bool enforce_security;
    
public:
    PluginManager();
    PluginManager(SecurityManager* sec_mgr);
    ~PluginManager();
    
    // Plugin loading
    bool loadPlugin(const std::string& plugin_path);
    bool loadPluginsFromDirectory(const std::string& directory);
    void unloadAllPlugins();
    
    // Plugin access
    std::vector<Plugin*> getLoadedPlugins() const;
    Plugin* getPluginByName(const std::string& name) const;
    
    // Metric collection
    std::map<std::string, std::vector<MetricData>> collectAllMetrics();
    std::vector<MetricData> collectMetricsFromPlugin(const std::string& plugin_name);
    
    // Display
    std::string formatPluginMetrics(bool use_colors = true);
    void listPlugins() const;
    
    // Security
    void setSecurityManager(SecurityManager* sec_mgr);
    void setSecurityEnforcement(bool enforce);
    bool isSecurityEnforced() const { return enforce_security; }
    
private:
    bool validatePlugin(Plugin* plugin);
    void unloadPlugin(Plugin* plugin);
};

// Helper macros for plugin development
#define DECLARE_PLUGIN(plugin_name, plugin_version, plugin_author, plugin_description) \
    extern "C" PluginInfo get_plugin_info() { \
        PluginInfo info; \
        info.name = plugin_name; \
        info.version = plugin_version; \
        info.author = plugin_author; \
        info.description = plugin_description; \
        info.api_version = SYSREPORT_PLUGIN_API_VERSION; \
        return info; \
    }

#define DECLARE_PLUGIN_INIT() \
    extern "C" bool init_plugin()

#define DECLARE_PLUGIN_CLEANUP() \
    extern "C" void cleanup_plugin()

#define DECLARE_PLUGIN_METRICS() \
    extern "C" std::vector<MetricData> get_metrics()

#endif // PLUGIN_H
