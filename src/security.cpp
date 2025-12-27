#include "security.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <ctime>

SecurityManager::SecurityManager() 
    : capabilities_dropped(false)
    , seccomp_enabled(false)
    , audit_log_path("/var/log/sysreport-security.log") {
}

bool SecurityManager::dropCapabilities() {
    // Simplified implementation - just mark as dropped
    capabilities_dropped = true;
    logSecurityEvent("INFO", "Capability dropping requested (simplified mode)");
    return true;
}

bool SecurityManager::setupSeccomp(const PluginSecurityPolicy& policy) {
    // Simplified implementation
    (void)policy;
    seccomp_enabled = true;
    logSecurityEvent("INFO", "Seccomp requested (simplified mode)");
    return true;
}

bool SecurityManager::validatePluginPath(const std::string& path) {
    // Check for path traversal attempts
    if (path.find("..") != std::string::npos) {
        logSecurityEvent("WARNING", "Path traversal attempt detected: " + path);
        return false;
    }
    
    // Check for null bytes
    if (path.find('\0') != std::string::npos) {
        logSecurityEvent("WARNING", "Null byte in path: " + path);
        return false;
    }
    
    // Ensure .so extension
    if (path.length() < 3 || path.substr(path.length() - 3) != ".so") {
        logSecurityEvent("WARNING", "Invalid plugin extension: " + path);
        return false;
    }
    
    // Check file exists and is regular file
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        logSecurityEvent("WARNING", "Plugin file not found: " + path);
        return false;
    }
    
    if (!S_ISREG(st.st_mode)) {
        logSecurityEvent("WARNING", "Plugin path is not a regular file: " + path);
        return false;
    }
    
    // Check file is not world-writable (security risk)
    if (st.st_mode & S_IWOTH) {
        logSecurityEvent("WARNING", "Plugin file is world-writable: " + path);
        return false;
    }
    
    return true;
}

bool SecurityManager::isPathAllowed(const std::string& path, const PluginSecurityPolicy& policy) {
    // Check denied paths first
    for (const auto& denied : policy.denied_paths) {
        if (path.find(denied) == 0) {
            logSecurityEvent("WARNING", "Access denied to path: " + path);
            return false;
        }
    }
    
    // If allowed paths list is empty, allow all (except denied)
    if (policy.allowed_paths.empty()) {
        return true;
    }
    
    // Check if path is in allowed list
    for (const auto& allowed : policy.allowed_paths) {
        if (path.find(allowed) == 0) {
            return true;
        }
    }
    
    logSecurityEvent("WARNING", "Path not in allowed list: " + path);
    return false;
}

bool SecurityManager::verifyPluginSignature(const std::string& plugin_path) {
    // TODO: Implement GPG signature verification
    (void)plugin_path;
    return true;
}

bool SecurityManager::setResourceLimits(const PluginSecurityPolicy& policy) {
    struct rlimit limit;
    
    // Set memory limit
    if (policy.max_memory_mb > 0) {
        limit.rlim_cur = policy.max_memory_mb * 1024 * 1024;
        limit.rlim_max = policy.max_memory_mb * 1024 * 1024;
        if (setrlimit(RLIMIT_AS, &limit) != 0) {
            logSecurityEvent("WARNING", "Failed to set memory limit");
        }
    }
    
    // Set file descriptor limit
    if (policy.max_file_descriptors > 0) {
        limit.rlim_cur = policy.max_file_descriptors;
        limit.rlim_max = policy.max_file_descriptors;
        if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
            logSecurityEvent("WARNING", "Failed to set file descriptor limit");
        }
    }
    
    logSecurityEvent("INFO", "Resource limits set");
    return true;
}

bool SecurityManager::createReadOnlyNamespace(const std::vector<std::string>& allowed_paths) {
    // TODO: Implement mount namespace creation
    (void)allowed_paths;
    logSecurityEvent("INFO", "Read-only namespace requested (not implemented yet)");
    return true;
}

void SecurityManager::logSecurityEvent(const std::string& level, const std::string& details) {
    std::ofstream log_file(audit_log_path, std::ios::app);
    if (log_file.is_open()) {
        time_t now = time(nullptr);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        log_file << "[" << timestamp << "] " << level << ": " << details << std::endl;
        log_file.close();
    }
    
    // Also log to stderr for visibility
    std::cerr << "Security [" << level << "]: " << details << std::endl;
}

PluginSecurityPolicy SecurityManager::getDefaultPolicy() {
    PluginSecurityPolicy policy;
    policy.allowed_permissions = {
        PluginPermission::READ_PROC,
        PluginPermission::READ_SYS
    };
    policy.enforce_sandboxing = false;
    policy.max_memory_mb = 100;
    policy.max_cpu_percent = 10;
    policy.max_file_descriptors = 64;
    policy.allow_network = false;
    policy.allowed_paths = {"/proc", "/sys"};
    policy.denied_paths = {"/etc/shadow", "/root", "/home"};
    return policy;
}

PluginSecurityPolicy SecurityManager::getRestrictedPolicy() {
    PluginSecurityPolicy policy;
    policy.allowed_permissions = {
        PluginPermission::READ_PROC
    };
    policy.enforce_sandboxing = true;
    policy.max_memory_mb = 50;
    policy.max_cpu_percent = 5;
    policy.max_file_descriptors = 32;
    policy.allow_network = false;
    policy.allowed_paths = {"/proc"};
    policy.denied_paths = {"/etc", "/root", "/home", "/sys"};
    return policy;
}

PluginSecurityPolicy SecurityManager::getPermissivePolicy() {
    PluginSecurityPolicy policy;
    policy.allowed_permissions = {
        PluginPermission::READ_PROC,
        PluginPermission::READ_SYS,
        PluginPermission::FILE_READ,
        PluginPermission::NETWORK
    };
    policy.enforce_sandboxing = false;
    policy.max_memory_mb = 500;
    policy.max_cpu_percent = 50;
    policy.max_file_descriptors = 256;
    policy.allow_network = true;
    policy.allowed_paths = {};
    policy.denied_paths = {"/etc/shadow", "/etc/gshadow"};
    return policy;
}

bool SecurityManager::isRoot() {
    return getuid() == 0 || geteuid() == 0;
}

bool SecurityManager::isSafePath(const std::string& path) {
    if (path.find("..") != std::string::npos) return false;
    if (path.find("//") != std::string::npos) return false;
    if (path.find('\0') != std::string::npos) return false;
    if (path.empty()) return false;
    return true;
}

// PrivilegeGuard implementation
PrivilegeGuard::PrivilegeGuard(bool elevate)
    : elevated(false)
    , original_uid(getuid())
    , original_gid(getgid()) {
    
    if (elevate && geteuid() == 0) {
        elevated = true;
    }
}

PrivilegeGuard::~PrivilegeGuard() {
    if (elevated) {
        if (setegid(original_gid) != 0) {
            std::cerr << "Warning: Failed to drop group privileges" << std::endl;
        }
        if (seteuid(original_uid) != 0) {
            std::cerr << "Warning: Failed to drop user privileges" << std::endl;
        }
    }
}
