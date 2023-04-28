#pragma once
#include <implot.h>
namespace metrics {
    struct MetricsTracker {
        MetricsTracker();
        ~MetricsTracker();
    };
    float getCurrentCPUUsage();
    int getCurrentVirtualMemoryUsage();
    int getCurrentPhysicalMemoryUsage();

    int getMaxSampleCount();
    ImPlotPoint getCPUSample(int idx, void* data);
    ImPlotPoint getPhysMemSample(int idx, void* data);
    ImPlotPoint getVirtMemSample(int idx, void* data);
};
