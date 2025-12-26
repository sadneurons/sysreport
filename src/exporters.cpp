#include "exporters.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// Prometheus Exporter Implementation
std::string PrometheusExporter::exportMetrics(const UtilizationInfo& util) {
    std::ostringstream oss;
    
    // CPU metrics
    oss << "# HELP cpu_usage_percent CPU usage percentage\n";
    oss << "# TYPE cpu_usage_percent gauge\n";
    oss << formatMetric("cpu_usage_percent", util.cpu_percent) << "\n";
    
    oss << "# HELP cpu_load_average_1m Load average over 1 minute\n";
    oss << "# TYPE cpu_load_average_1m gauge\n";
    oss << formatMetric("cpu_load_average_1m", util.load_avg_1) << "\n";
    
    oss << "# HELP cpu_load_average_5m Load average over 5 minutes\n";
    oss << "# TYPE cpu_load_average_5m gauge\n";
    oss << formatMetric("cpu_load_average_5m", util.load_avg_5) << "\n";
    
    oss << "# HELP cpu_load_average_15m Load average over 15 minutes\n";
    oss << "# TYPE cpu_load_average_15m gauge\n";
    oss << formatMetric("cpu_load_average_15m", util.load_avg_15) << "\n";
    
    // Per-core CPU
    for (size_t i = 0; i < util.cpu_per_core.size(); i++) {
        std::string labels = "core=\"" + std::to_string(i) + "\"";
        oss << formatMetric("cpu_core_usage_percent", util.cpu_per_core[i], labels) << "\n";
    }
    
    // Memory metrics
    oss << "\n# HELP memory_usage_percent Memory usage percentage\n";
    oss << "# TYPE memory_usage_percent gauge\n";
    oss << formatMetric("memory_usage_percent", util.ram_percent) << "\n";
    
    oss << "# HELP memory_used_bytes Memory used in bytes\n";
    oss << "# TYPE memory_used_bytes gauge\n";
    long mem_used_bytes = static_cast<long>(util.used_ram_mb) * 1024L * 1024L;
    oss << formatMetric("memory_used_bytes", mem_used_bytes) << "\n";
    
    oss << "# HELP memory_available_bytes Memory available in bytes\n";
    oss << "# TYPE memory_available_bytes gauge\n";
    long mem_avail_bytes = static_cast<long>(util.available_ram_mb) * 1024L * 1024L;
    oss << formatMetric("memory_available_bytes", mem_avail_bytes) << "\n";
    
    // Swap metrics
    if (util.used_swap_mb > 0) {
        oss << "\n# HELP swap_usage_percent Swap usage percentage\n";
        oss << "# TYPE swap_usage_percent gauge\n";
        oss << formatMetric("swap_usage_percent", util.swap_percent) << "\n";
        
        oss << "# HELP swap_used_bytes Swap used in bytes\n";
        oss << "# TYPE swap_used_bytes gauge\n";
        long swap_bytes = static_cast<long>(util.used_swap_mb) * 1024L * 1024L;
        oss << formatMetric("swap_used_bytes", swap_bytes) << "\n";
    }
    
    // Disk metrics
    oss << "\n# HELP disk_usage_percent Disk usage percentage\n";
    oss << "# TYPE disk_usage_percent gauge\n";
    for (const auto& disk : util.disks) {
        std::string labels = "mount=\"" + disk.mount_point + "\",device=\"" + disk.device + "\"";
        oss << formatMetric("disk_usage_percent", disk.percent, labels) << "\n";
        
        std::string used_labels = labels;
        long used_bytes = static_cast<long>(disk.used_gb) * 1024L * 1024L * 1024L;
        oss << formatMetric("disk_used_bytes", used_bytes, used_labels) << "\n";
    }
    
    // Network metrics
    oss << "\n# HELP network_rx_bytes_total Network received bytes\n";
    oss << "# TYPE network_rx_bytes_total counter\n";
    oss << "# HELP network_tx_bytes_total Network transmitted bytes\n";
    oss << "# TYPE network_tx_bytes_total counter\n";
    for (const auto& net : util.network) {
        std::string labels = "interface=\"" + net.interface + "\"";
        oss << formatMetric("network_rx_bytes_total", net.rx_bytes, labels) << "\n";
        oss << formatMetric("network_tx_bytes_total", net.tx_bytes, labels) << "\n";
    }
    
    // GPU metrics
    if (!util.gpus.empty() && util.gpus[0].available) {
        oss << "\n# HELP gpu_utilization_percent GPU utilization percentage\n";
        oss << "# TYPE gpu_utilization_percent gauge\n";
        std::string labels = "name=\"" + util.gpus[0].name + "\",vendor=\"" + util.gpus[0].vendor + "\"";
        oss << formatMetric("gpu_utilization_percent", util.gpus[0].utilization_percent, labels) << "\n";
        
        oss << "# HELP gpu_temperature_celsius GPU temperature in celsius\n";
        oss << "# TYPE gpu_temperature_celsius gauge\n";
        oss << formatMetric("gpu_temperature_celsius", util.gpus[0].temperature, labels) << "\n";
        
        oss << "# HELP gpu_memory_used_bytes GPU memory used in bytes\n";
        oss << "# TYPE gpu_memory_used_bytes gauge\n";
        long gpu_mem_bytes = static_cast<long>(util.gpus[0].memory_used_mb) * 1024L * 1024L;
        oss << formatMetric("gpu_memory_used_bytes", gpu_mem_bytes, labels) << "\n";
    }
    
    // Battery metrics
    if (util.battery.present) {
        oss << "\n# HELP battery_charge_percent Battery charge percentage\n";
        oss << "# TYPE battery_charge_percent gauge\n";
        oss << formatMetric("battery_charge_percent", util.battery.percent) << "\n";
        
        oss << "# HELP battery_charging Battery charging status (1=charging, 0=discharging)\n";
        oss << "# TYPE battery_charging gauge\n";
        oss << formatMetric("battery_charging", util.battery.charging ? 1.0 : 0.0) << "\n";
    }
    
    // Temperature metrics
    if (!util.temperatures.empty()) {
        oss << "\n# HELP cpu_temperature_celsius CPU temperature in celsius\n";
        oss << "# TYPE cpu_temperature_celsius gauge\n";
        for (size_t i = 0; i < util.temperatures.size(); i++) {
            std::string labels = "sensor=\"" + std::to_string(i) + "\"";
            oss << formatMetric("cpu_temperature_celsius", util.temperatures[i], labels) << "\n";
        }
    }
    
    return oss.str();
}

std::string PrometheusExporter::formatMetric(const std::string& name, double value, 
                                              const std::string& labels) {
    std::ostringstream oss;
    oss << name;
    if (!labels.empty()) {
        oss << "{" << labels << "}";
    }
    oss << " " << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

std::string PrometheusExporter::formatMetric(const std::string& name, long value, 
                                              const std::string& labels) {
    std::ostringstream oss;
    oss << name;
    if (!labels.empty()) {
        oss << "{" << labels << "}";
    }
    oss << " " << value;
    return oss.str();
}

// InfluxDB Exporter Implementation
std::string InfluxDBExporter::exportMetrics(const UtilizationInfo& util, 
                                            const std::string& measurement) {
    std::ostringstream oss;
    long timestamp = getCurrentTimestampNs();
    
    // CPU metrics
    std::ostringstream cpu_fields;
    cpu_fields << "cpu_percent=" << std::fixed << std::setprecision(2) << util.cpu_percent
               << ",load_1m=" << util.load_avg_1
               << ",load_5m=" << util.load_avg_5
               << ",load_15m=" << util.load_avg_15;
    oss << formatPoint(measurement + "_cpu", cpu_fields.str(), "", timestamp) << "\n";
    
    // Memory metrics
    std::ostringstream mem_fields;
    mem_fields << "usage_percent=" << std::fixed << std::setprecision(2) << util.ram_percent
               << ",used_bytes=" << (util.used_ram_mb * 1024LL * 1024)
               << ",available_bytes=" << (util.available_ram_mb * 1024LL * 1024);
    oss << formatPoint(measurement + "_memory", mem_fields.str(), "", timestamp) << "\n";
    
    // Swap metrics
    if (util.used_swap_mb > 0) {
        std::ostringstream swap_fields;
        swap_fields << "usage_percent=" << std::fixed << std::setprecision(2) << util.swap_percent
                   << ",used_bytes=" << (util.used_swap_mb * 1024LL * 1024);
        oss << formatPoint(measurement + "_swap", swap_fields.str(), "", timestamp) << "\n";
    }
    
    // Disk metrics
    for (const auto& disk : util.disks) {
        std::ostringstream disk_fields;
        disk_fields << "usage_percent=" << std::fixed << std::setprecision(2) << disk.percent
                   << ",used_bytes=" << (disk.used_gb * 1024LL * 1024 * 1024);
        std::string tags = "mount=" + disk.mount_point + ",device=" + disk.device;
        oss << formatPoint(measurement + "_disk", disk_fields.str(), tags, timestamp) << "\n";
    }
    
    // Network metrics
    for (const auto& net : util.network) {
        std::ostringstream net_fields;
        net_fields << "rx_bytes=" << net.rx_bytes << ",tx_bytes=" << net.tx_bytes;
        if (net.rx_mbps > 0 || net.tx_mbps > 0) {
            net_fields << ",rx_mbps=" << std::fixed << std::setprecision(2) << net.rx_mbps
                      << ",tx_mbps=" << net.tx_mbps;
        }
        std::string tags = "interface=" + net.interface;
        oss << formatPoint(measurement + "_network", net_fields.str(), tags, timestamp) << "\n";
    }
    
    // GPU metrics
    if (!util.gpus.empty() && util.gpus[0].available) {
        std::ostringstream gpu_fields;
        gpu_fields << "utilization_percent=" << std::fixed << std::setprecision(2) 
                  << util.gpus[0].utilization_percent
                  << ",temperature=" << util.gpus[0].temperature
                  << ",memory_used_mb=" << util.gpus[0].memory_used_mb;
        std::string tags = "name=" + util.gpus[0].name + ",vendor=" + util.gpus[0].vendor;
        oss << formatPoint(measurement + "_gpu", gpu_fields.str(), tags, timestamp) << "\n";
    }
    
    // Battery metrics
    if (util.battery.present) {
        std::ostringstream bat_fields;
        bat_fields << "charge_percent=" << std::fixed << std::setprecision(2) << util.battery.percent
                  << ",charging=" << (util.battery.charging ? "true" : "false");
        oss << formatPoint(measurement + "_battery", bat_fields.str(), "", timestamp) << "\n";
    }
    
    return oss.str();
}

std::string InfluxDBExporter::formatPoint(const std::string& measurement,
                                          const std::string& fields,
                                          const std::string& tags,
                                          long timestamp_ns) {
    std::ostringstream oss;
    oss << measurement;
    if (!tags.empty()) {
        oss << "," << tags;
    }
    oss << " " << fields;
    if (timestamp_ns > 0) {
        oss << " " << timestamp_ns;
    }
    return oss.str();
}

long InfluxDBExporter::getCurrentTimestampNs() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

// Webhook Client Implementation
WebhookClient::WebhookClient(const std::string& webhook_url, int timeout) 
    : url(webhook_url), timeout_seconds(timeout) {
}

bool WebhookClient::sendAlert(const std::string& metric, double value, double threshold,
                              const std::string& severity) {
    std::string payload = createAlertPayload(metric, value, threshold, severity);
    return sendCustomPayload(payload);
}

bool WebhookClient::sendCustomPayload(const std::string& json_payload) {
    // Simple HTTP POST implementation using sockets
    // Parse URL to extract host and path
    std::string host, path;
    size_t proto_end = url.find("://");
    if (proto_end == std::string::npos) return false;
    
    size_t host_start = proto_end + 3;
    size_t path_start = url.find("/", host_start);
    
    if (path_start != std::string::npos) {
        host = url.substr(host_start, path_start - host_start);
        path = url.substr(path_start);
    } else {
        host = url.substr(host_start);
        path = "/";
    }
    
    // Extract port if specified
    int port = 80;
    size_t colon_pos = host.find(":");
    if (colon_pos != std::string::npos) {
        port = std::stoi(host.substr(colon_pos + 1));
        host = host.substr(0, colon_pos);
    }
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;
    
    // Set timeout
    struct timeval tv;
    tv.tv_sec = timeout_seconds;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    
    // Resolve hostname
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        close(sock);
        return false;
    }
    
    // Connect
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return false;
    }
    
    // Build HTTP request
    std::ostringstream request;
    request << "POST " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Content-Type: application/json\r\n";
    request << "Content-Length: " << json_payload.length() << "\r\n";
    request << "Connection: close\r\n";
    request << "\r\n";
    request << json_payload;
    
    // Send request
    std::string req_str = request.str();
    ssize_t sent = send(sock, req_str.c_str(), req_str.length(), 0);
    
    close(sock);
    return sent > 0;
}

std::string WebhookClient::createAlertPayload(const std::string& metric, double value,
                                              double threshold, const std::string& severity) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"metric\": \"" << metric << "\",\n";
    oss << "  \"value\": " << std::fixed << std::setprecision(2) << value << ",\n";
    oss << "  \"threshold\": " << threshold << ",\n";
    oss << "  \"severity\": \"" << severity << "\",\n";
    oss << "  \"timestamp\": " << std::time(nullptr) << ",\n";
    oss << "  \"hostname\": \"";
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        oss << hostname;
    }
    oss << "\"\n";
    oss << "}";
    return oss.str();
}
