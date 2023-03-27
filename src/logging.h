#pragma once
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"

namespace logging {
    struct LogManager {
        LogManager();
        ~LogManager();
    };

    // log fatal error and quit
    template<typename... Args>
    void logFatal(const char* fmt, const Args&... args) {
        extern std::unique_ptr<spdlog::logger> logger;
        logger->critical(fmt, args...);
        scrollToBot = true;
    }

    // log error and continue execution
    template<typename... Args>
    void logErr(const char* fmt, const Args&... args) {
        extern std::unique_ptr<spdlog::logger> logger;
        logger->error(fmt, args...);
        scrollToBot = true;
    }

    // log warning
    template<typename... Args>
    void logWarn(const char* fmt, const Args&... args) {
        extern std::unique_ptr<spdlog::logger> logger;
        logger->warn(fmt, args...);
        scrollToBot = true;
    }

    // log info
    template<typename... Args>
    void logInfo(const char* fmt, const Args&... args) {
        extern std::unique_ptr<spdlog::logger> logger;
        logger->info(fmt, args...);
        scrollToBot = true;
    }

    extern bool scrollToBot;

    const char* getLogString();

    //change severity level
    enum ELogLevel {
        LOG_CRITICAL,
        LOG_ERROR,
        LOG_WARNING,
        LOG_INFO
    };
    void setSeverityLevel(ELogLevel level);
}