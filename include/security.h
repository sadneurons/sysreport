#ifndef SECURITY_H
#define SECURITY_H

#include <string>
#include <vector>
#include <set>

// Plugin permission flags
enum class PluginPermission {
    READ_PROC,          // Read /proc filesystem
    READ_SYS,           // Read /sys filesystem
    NETWORK,            // Network access
    EXECUTE,            // Execute external commands
    FILE_READ,          // Read arbitrary files
    FILE_WRITE,         // Write to files
    ALL                 // All permissions (unsafe)
};

// Security policy for plugins
struct PluginSecurityPolicy {
    std::set<PluginPermission> allowed_permissions;
    bool enforce_sandboxing;
    size_t max_memory_mb;
    size_t max_cpu_percent;
    size_t max_file_descriptors;
    bool allow_network;
    std::vector<std::string> allowed_paths;
    std::vector<std::string> denied_paths;
};

// Security manager class
class SecurityManager {
public:
    SecurityManager();
    
    // Drop unnecessary capabilities
    bool dropCapabilities();
    
    // Set up seccomp filter for sandboxing
    bool setupSeccomp(const PluginSecurityPolicy& policy);
    
    // Validate plugin path is safe
    bool validatePluginPath(const std::string& path);
    
    // Check if path is allowed for plugin access
    bool isPathAllowed(const std::string& path, const PluginSecurityPolicy& policy);
    
    // Verify plugin signature (future: implement GPG signature checking)
    bool verifyPluginSignature(const std::string& plugin_path);
    
    // Set resource limits for plugin execution
    bool setResourceLimits(const PluginSecurityPolicy& policy);
    
    // Create read-only mount namespace
    bool createReadOnlyNamespace(const std::vector<std::string>& allowed_paths);
    
    // Log security event
    void logSecurityEvent(const std::string& event, const std::string& details);
    
    // Get default security policy
    static PluginSecurityPolicy getDefaultPolicy();
    
    // Get restricted security policy (minimal permissions)
    static PluginSecurityPolicy getRestrictedPolicy();
    
    // Get permissive security policy (for trusted plugins)
    static PluginSecurityPolicy getPermissivePolicy();
    
private:
    bool capabilities_dropped;
    bool seccomp_enabled;
    std::string audit_log_path;
    
    // Helper: check if running as root
    bool isRoot();
    
    // Helper: validate file path doesn't contain dangerous patterns
    bool isSafePath(const std::string& path);
};

// RAII wrapper for temporary privilege elevation
class PrivilegeGuard {
public:
    PrivilegeGuard(bool elevate = false);
    ~PrivilegeGuard();
    
    bool isElevated() const { return elevated; }
    
private:
    bool elevated;
    uid_t original_uid;
    gid_t original_gid;
};

#endif // SECURITY_H
