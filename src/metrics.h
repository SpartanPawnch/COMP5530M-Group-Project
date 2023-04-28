#pragma once
namespace metrics {
    struct MetricsTracker {
        MetricsTracker();
        ~MetricsTracker();
    };
    float getCurrentCPUUsage();
    int getCurrentVirtualMemoryUsage();
    int getCurrentPhysicalMemoryUsage();
    // TODO graph data
};
