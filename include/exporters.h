#ifndef EXPORTERS_H
#define EXPORTERS_H

#include "system_info.h"
#include <string>

// Prometheus text format exporter
class PrometheusExporter {
public:
    static std::string exportMetrics(const UtilizationInfo& util);
    static std::string formatMetric(const std::string& name, double value, 
                                    const std::string& labels = "");
    static std::string formatMetric(const std::string& name, long value, 
                                    const std::string& labels = "");
};

// InfluxDB line protocol exporter
class InfluxDBExporter {
public:
    static std::string exportMetrics(const UtilizationInfo& util, 
                                     const std::string& measurement = "sysreport");
    static std::string formatPoint(const std::string& measurement,
                                   const std::string& fields,
                                   const std::string& tags = "",
                                   long timestamp_ns = 0);
private:
    static long getCurrentTimestampNs();
};

// Webhook integration
class WebhookClient {
private:
    std::string url;
    int timeout_seconds;
    
public:
    WebhookClient(const std::string& webhook_url, int timeout = 5);
    
    bool sendAlert(const std::string& metric, double value, double threshold,
                  const std::string& severity = "warning");
    bool sendCustomPayload(const std::string& json_payload);
    
private:
    std::string createAlertPayload(const std::string& metric, double value,
                                   double threshold, const std::string& severity);
};

#endif // EXPORTERS_H
