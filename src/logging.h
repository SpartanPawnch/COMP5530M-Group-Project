#pragma once

namespace logging {
    struct LogManager {
        LogManager();
        ~LogManager();
    };

    extern bool scrollToBot;

    // log fatal error and quit
    void logFatal(const char* fmt, ...);

    // log error and continue execution
    void logErr(const char* fmt, ...);

    // log warning
    void logWarn(const char* fmt, ...);

    // log info
    void logInfo(const char* fmt, ...);

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