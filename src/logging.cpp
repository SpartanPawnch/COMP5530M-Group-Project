#include "logging.h"

#include <string>
#include <sstream>
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"

namespace logging {
    static std::ostringstream logOss;
    static std::shared_ptr<spdlog::sinks::ostream_sink_mt> logSink;
    std::unique_ptr<spdlog::logger> logger;
    bool scrollToBot = false;

    LogManager::LogManager() {
        logSink = std::make_shared<spdlog::sinks::ostream_sink_mt>(logOss);
        logger = std::make_unique<spdlog::logger>("default", logSink);
        logger->set_level(spdlog::level::info);
        logger->set_pattern("%v");
    }
    LogManager::~LogManager() {
    }

    static std::string logString;
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