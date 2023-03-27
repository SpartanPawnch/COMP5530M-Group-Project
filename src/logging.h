#pragma once

namespace logging {
    struct LogManager {
        LogManager();
        ~LogManager();
    };

    extern bool scrollToBot;

    // log fatal error and quit
    void logFatal(const char* msg);

    // log error and continue execution
    void logErr(const char* msg);

    // log warning
    void logWarn(const char* msg);

    // log info
    void logInfo(const char* msg);

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