#pragma once
#include <implot.h>
namespace metrics {
    struct MetricsTracker {
        MetricsTracker();
        ~MetricsTracker();
    };
    // CPU Metrics
    float getCurrentCPUUsage();
    float getMinCPUUsage();
    float getMaxCPUUsage();

    // Virtual Memory metrics
    int getCurrentVirtualMemoryUsage();
    int getMinVirtualMemoryUsage();
    int getMaxVirtualMemoryUsage();

    // Physical Memory metrics
    int getCurrentPhysicalMemoryUsage();
    int getMinPhysicalMemoryUsage();
    int getMaxPhysicalMemoryUsage();

    // Getters for plotting
    int getMaxSampleCount();
    ImPlotPoint getCPUSample(int idx, void* data);
    ImPlotPoint getPhysMemSample(int idx, void* data);
    ImPlotPoint getVirtMemSample(int idx, void* data);
};
