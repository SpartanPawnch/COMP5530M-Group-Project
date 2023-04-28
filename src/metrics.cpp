#include "metrics.h"
#include <windows.h>
#include <psapi.h>
#include <stdint.h>
#include <thread>
#include <algorithm>

#include <implot.h>

namespace metrics {
    // previous sample state
    static FILETIME lastCPU, lastSysCPU, lastUserCPU;

    // process info
    static int numProcessors;
    static HANDLE currProc;

    // ring buffer data
    const int MAX_SAMPLE_COUNT = 100;
    static int samplesTaken = 0;
    static int currOffset = 0;
    static float cpuSamples[MAX_SAMPLE_COUNT] = {.0f};
    static float physMemSamples[MAX_SAMPLE_COUNT] = {.0f};
    static float virtMemSamples[MAX_SAMPLE_COUNT] = {.0f};

    // CPU stats
    static float currCPUUsage = .0f;

    // Memory stats
    static int currPhysMemUsage = 0;
    static int currVirtMemUsage = 0;

    std::thread trackerThread;
    static bool stopTracker = false;

    static int64_t filetimeToInt(const FILETIME& ft) {
        return (int64_t(ft.dwHighDateTime) * (1ll << 32) + ft.dwLowDateTime);
    }

    ImPlotPoint getCPUSample(int idx, void* data) {
        return ImPlotPoint(float(idx), cpuSamples[(currOffset + idx) % MAX_SAMPLE_COUNT]);
    }

    ImPlotPoint getPhysMemSample(int idx, void* data) {
        return (ImPlotPoint(float(idx),
            float(physMemSamples[(currOffset + idx) % MAX_SAMPLE_COUNT] / (1024.f * 1024.f))));
    }

    ImPlotPoint getVirtMemSample(int idx, void* data) {
        return (ImPlotPoint(float(idx),
            float(virtMemSamples[(currOffset + idx) % MAX_SAMPLE_COUNT]) / (1024.f * 1024.f)));
    }

    static void trackMetrics() {
        static const int POLL_INTERVAL_MS = 50;
        while (!stopTracker) {
            // get raw cpu metrics
            FILETIME currTime, currSys, currUser;
            GetSystemTimeAsFileTime(&currTime);
            FILETIME ignore;
            GetProcessTimes(currProc, &ignore, &ignore, &currSys, &currUser);

            // calculate usage %
            currCPUUsage = filetimeToInt(currUser) - filetimeToInt(lastUserCPU) +
                filetimeToInt(currSys) - filetimeToInt(lastSysCPU);

            currCPUUsage /= (filetimeToInt(currTime) - filetimeToInt(lastCPU)) * numProcessors;
            currCPUUsage *= 100.f;

            // update previous metrics
            lastCPU = currTime;
            lastSysCPU = currSys;
            lastUserCPU = currUser;

            PROCESS_MEMORY_COUNTERS_EX memInfo;
            GetProcessMemoryInfo(currProc, (PROCESS_MEMORY_COUNTERS*)&memInfo, sizeof(memInfo));
            currVirtMemUsage = int(memInfo.PrivateUsage);
            currPhysMemUsage = int(memInfo.WorkingSetSize);

            // update sampled data
            cpuSamples[samplesTaken % MAX_SAMPLE_COUNT] = currCPUUsage;
            physMemSamples[samplesTaken % MAX_SAMPLE_COUNT] = currPhysMemUsage;
            virtMemSamples[samplesTaken % MAX_SAMPLE_COUNT] = currVirtMemUsage;
            samplesTaken++;
            currOffset = (currOffset + int(samplesTaken >= MAX_SAMPLE_COUNT)) % MAX_SAMPLE_COUNT;

            Sleep(POLL_INTERVAL_MS);
        }
    }

    MetricsTracker::MetricsTracker() {
        // get process handle
        currProc = GetCurrentProcess();

        // init cpu metrics
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&lastCPU);

        FILETIME ignore;
        GetProcessTimes(currProc, &ignore, &ignore, &lastSysCPU, &lastUserCPU);
        trackerThread = std::thread(trackMetrics);
    }

    MetricsTracker::~MetricsTracker() {
        stopTracker = true;
        if (trackerThread.joinable())
            trackerThread.join();
    }

    float getCurrentCPUUsage() {

        return currCPUUsage;
    }

    int getCurrentVirtualMemoryUsage() {
        return currVirtMemUsage;
    }

    int getCurrentPhysicalMemoryUsage() {
        return currPhysMemUsage;
    }
    int getMaxSampleCount() {
        return std::min<int>(MAX_SAMPLE_COUNT, samplesTaken);
    }
};
