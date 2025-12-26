# Plugin Development Guide

## Overview

The sysreport plugin system allows you to extend sysreport with custom metrics and functionality through dynamically loaded shared libraries (.so files).

## Plugin API

### API Version

Current API version: **1**

All plugins must return this API version in their `PluginInfo` structure for compatibility checking.

### Required Functions

Every plugin must implement these three functions:

```cpp
extern "C" PluginInfo get_plugin_info();
extern "C" bool init_plugin();
extern "C" void cleanup_plugin();
extern "C" std::vector<MetricData> get_metrics();
```

### Data Structures

#### PluginInfo
```cpp
struct PluginInfo {
    std::string name;           // Plugin name
    std::string version;        // Plugin version (semantic versioning)
    std::string author;         // Author name
    std::string description;    // Short description
    int api_version;            // Must be SYSREPORT_PLUGIN_API_VERSION
};
```

#### MetricData
```cpp
struct MetricData {
    std::string name;           // Metric name (e.g., "CPU Temperature")
    std::string value;          // Metric value as string
    std::string unit;           // Unit (e.g., "°C", "MB", "s")
    std::string description;    // Optional description
};
```

## Helper Macros

### DECLARE_PLUGIN

Simplifies plugin metadata declaration:

```cpp
DECLARE_PLUGIN(
    "My Plugin",                    // name
    "1.0.0",                        // version
    "Your Name",                    // author
    "Description of what it does"   // description
)
```

This macro automatically:
- Creates the `get_plugin_info()` function
- Sets the correct API version
- Returns a properly initialized `PluginInfo` structure

### DECLARE_PLUGIN_INIT

Declares the initialization function:

```cpp
DECLARE_PLUGIN_INIT() {
    // Your initialization code here
    return true;  // Return false if initialization fails
}
```

### DECLARE_PLUGIN_CLEANUP

Declares the cleanup function:

```cpp
DECLARE_PLUGIN_CLEANUP() {
    // Your cleanup code here
}
```

### DECLARE_PLUGIN_METRICS

Declares the metrics collection function:

```cpp
DECLARE_PLUGIN_METRICS() {
    std::vector<MetricData> metrics;
    
    // Add your metrics
    metrics.push_back({"Metric Name", "Value", "Unit", "Description"});
    
    return metrics;
}
```

## Complete Example

Here's a complete example plugin that reports system uptime:

```cpp
#include "../include/plugin.h"
#include <sys/sysinfo.h>
#include <sstream>
#include <iomanip>

// Plugin metadata
DECLARE_PLUGIN("System Uptime", "1.0.0", "Sysreport Team", "Provides system uptime information")

// Plugin initialization
DECLARE_PLUGIN_INIT() {
    return true;  // No initialization needed
}

// Plugin cleanup
DECLARE_PLUGIN_CLEANUP() {
    // No cleanup needed
}

// Collect metrics
DECLARE_PLUGIN_METRICS() {
    std::vector<MetricData> metrics;
    
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        return metrics;  // Return empty on error
    }
    
    // Calculate uptime
    long uptime_seconds = info.uptime;
    int days = uptime_seconds / (24 * 3600);
    uptime_seconds %= (24 * 3600);
    int hours = uptime_seconds / 3600;
    uptime_seconds %= 3600;
    int minutes = uptime_seconds / 60;
    
    std::ostringstream oss;
    if (days > 0) oss << days << "d ";
    oss << hours << "h " << minutes << "m";
    
    metrics.push_back({
        "Uptime",
        oss.str(),
        "",
        "Total system uptime"
    });
    
    metrics.push_back({
        "Uptime (seconds)",
        std::to_string(info.uptime),
        "s",
        "Raw uptime in seconds"
    });
    
    return metrics;
}
```

## Building Plugins

### Compilation Flags

Plugins must be compiled with specific flags:

```bash
g++ -std=c++17 -Wall -Wextra -fPIC -I../include -shared myplugin.cpp -o myplugin.so
```

**Required flags:**
- `-std=c++17`: Use C++17 standard
- `-fPIC`: Position-independent code (required for shared libraries)
- `-shared`: Create a shared library
- `-I../include`: Include path to plugin.h

### Using the Plugin Makefile

The `plugins/Makefile` provides a template:

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -I../include
LDFLAGS = -shared

all: build/myplugin.so

build/myplugin.so: myplugin.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -rf build

install:
	mkdir -p /usr/lib/sysreport/plugins/
	cp build/*.so /usr/lib/sysreport/plugins/
```

## Loading Plugins

### From Command Line

Load a single plugin:
```bash
sysreport --plugin ./myplugin.so
```

Load all plugins from a directory:
```bash
sysreport --plugin-dir ./plugins/
```

List loaded plugins:
```bash
sysreport --plugin-dir ./plugins/ --list-plugins
```

### Plugin Search Paths

Plugins can be loaded from:
1. Absolute paths: `/path/to/plugin.so`
2. Relative paths: `./plugins/plugin.so`
3. Plugin directories specified with `--plugin-dir`

## Best Practices

### Error Handling

Always check for errors and return empty/default values gracefully:

```cpp
DECLARE_PLUGIN_METRICS() {
    std::vector<MetricData> metrics;
    
    // Check for errors
    if (some_operation_failed()) {
        return metrics;  // Return empty vector
    }
    
    // Add metrics...
    return metrics;
}
```

### Resource Management

Clean up resources in `cleanup_plugin()`:

```cpp
static FILE* log_file = nullptr;

DECLARE_PLUGIN_INIT() {
    log_file = fopen("/tmp/plugin.log", "a");
    return (log_file != nullptr);
}

DECLARE_PLUGIN_CLEANUP() {
    if (log_file) {
        fclose(log_file);
        log_file = nullptr;
    }
}
```

### Performance

- Keep metric collection fast (< 100ms recommended)
- Cache expensive operations when possible
- Avoid blocking I/O in metric collection

### Naming Conventions

- **Plugin names**: Descriptive, title case (e.g., "System Uptime")
- **Metric names**: Clear, concise (e.g., "CPU Temperature")
- **Units**: Standard abbreviations (°C, MB, GB, %, s, etc.)

## Debugging

### Check Plugin Loading

Use `--list-plugins` to verify your plugin loads correctly:

```bash
sysreport --plugin ./myplugin.so --list-plugins
```

Expected output:
```
Loaded plugin: My Plugin v1.0.0
Loaded 1 plugin(s)

Loaded Plugins:
===============

My Plugin v1.0.0
  Author:      Your Name
  Description: Description of what it does
  Path:        ./myplugin.so
  API Version: 1
  Status:      Initialized
```

### Common Issues

**Plugin not loading:**
- Check that the .so file exists and has execute permissions
- Verify all required symbols are exported (`nm -D plugin.so`)
- Ensure API version matches

**Plugin loads but no metrics:**
- Check `get_metrics()` is exported
- Verify it returns a non-empty vector
- Check for runtime errors in metric collection

**Crashes:**
- Check for memory errors (use valgrind)
- Ensure all pointers are initialized
- Verify API compatibility

### Symbol Checking

Check exported symbols:
```bash
nm -D myplugin.so | grep -E "get_plugin_info|init_plugin|cleanup_plugin|get_metrics"
```

Should show:
```
0000000000001234 T get_metrics
0000000000001567 T get_plugin_info  
000000000000189a T init_plugin
0000000000001a2c T cleanup_plugin
```

## API Compatibility

### Version Checking

The plugin manager checks API versions automatically. If your plugin's API version doesn't match, it will not load.

### Future Compatibility

- The plugin API may evolve with new versions
- Old plugins will continue to work with their declared API version
- New features will require higher API versions

## Example Plugins

See the `plugins/` directory for working examples:

1. **example_uptime.cpp** - System uptime metrics
2. **example_custom.cpp** - User/system information

## Contributing

When submitting plugins:
1. Follow the coding style of example plugins
2. Include clear documentation in comments
3. Test thoroughly on target systems
4. Provide a README with dependencies and usage

## License

Plugins are independent works and can use any license. The sysreport plugin API is MIT licensed.
