#include "logging.h"

#include <string>
#include <sstream>
#include <memory>
#include <cstdarg>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"

namespace logging {
    static std::string logString;
    static std::ostringstream logOss;
    static std::shared_ptr<spdlog::sinks::ostream_sink_mt> logSink;
    static std::unique_ptr<spdlog::logger> logger;
    bool scrollToBot = false;

    LogManager::LogManager() {
        logSink = std::make_shared<spdlog::sinks::ostream_sink_mt>(logOss);
        logger = std::make_unique<spdlog::logger>("default", logSink);
        logger->set_level(spdlog::level::info);
        logger->set_pattern("%v");
    }
    LogManager::~LogManager() {
    }

    // log fatal error and quit
    void logFatal(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logger->critical(fmt, args);
        va_end(args);
    }

    // log error and continue execution
    void logErr(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logger->error(fmt, args);
        va_end(args);
    }

    // log warning
    void logWarn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logger->warn(fmt, args);
        va_end(args);
    }

    // log info
    void logInfo(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logger->info(fmt, args);
        va_end(args);
    }

    const char* getLogString() {
        logString = logOss.str();
        return logString.c_str();
    }

    void setSeverityLevel(ELogLevel level) {
        switch (level) {
        case LOG_CRITICAL:
            logger->set_level(spdlog::level::critical);
            break;
        case LOG_ERROR:
            logger->set_level(spdlog::level::err);
            break;
        case LOG_WARNING:
            logger->set_level(spdlog::level::warn);
            break;
        case LOG_INFO:
            logger->set_level(spdlog::level::info);
        }
    }
}