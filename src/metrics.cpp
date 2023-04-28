#include "metrics.h"
#include <windows.h>
#include <psapi.h>
#include <stdint.h>
#include <thread>
namespace metrics {
    static FILETIME lastCPU, lastSysCPU, lastUserCPU;
    static int numProcessors;
    static HANDLE currProc;
    static float currCPUUsage = .0f;
    static int currPhysMemUsage = 0;
    static int currVirtMemUsage = 0;

    std::thread trackerThread;
    static bool stopTracker = false;

    static void trackMetrics() {
        static const int POLL_INTERVAL_MS = 1000;
        while (!stopTracker) {
            // get raw cpu metrics
            FILETIME currTime, currSys, currUser;
            GetSystemTimeAsFileTime(&currTime);
            FILETIME ignore;
            GetProcessTimes(currProc, &ignore, &ignore, &currSys, &currUser);

            // calculate usage %
            currCPUUsage = currUser.dwLowDateTime - lastUserCPU.dwLowDateTime +
                currSys.dwLowDateTime - lastSysCPU.dwLowDateTime;

            currCPUUsage /= (currTime.dwLowDateTime - lastCPU.dwLowDateTime) * numProcessors;
            currCPUUsage *= 100.f;

            // update previous metrics
            lastCPU = currTime;
            lastSysCPU = currSys;
            lastUserCPU = currUser;

            PROCESS_MEMORY_COUNTERS_EX memInfo;
            GetProcessMemoryInfo(currProc, (PROCESS_MEMORY_COUNTERS*)&memInfo, sizeof(memInfo));
            currVirtMemUsage = int(memInfo.PrivateUsage);
            currPhysMemUsage = int(memInfo.PagefileUsage);

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
};
