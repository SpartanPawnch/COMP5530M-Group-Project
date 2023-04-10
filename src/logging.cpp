#include "logging.h"

#include <string>
#include <sstream>
#include <memory>

#include <lua.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"

#include "scripting.h"

namespace logging {
    static std::ostringstream logOss;
    static std::shared_ptr<spdlog::sinks::ostream_sink_mt> logSink;
    std::unique_ptr<spdlog::logger> logger;
    bool scrollToBot = false;

    // --- Lua Function Definitions ---
    static int luaLogInfo(lua_State* L) {
        int argc = lua_gettop(L);
        for (int i = 0; i < argc; i++) {
            size_t len;
            const char* msg = lua_tolstring(L, i, &len);
            logInfo("{}", msg);
        }

        return 0;
    }

    // --- Module Implementations
    LogManager::LogManager() {
        // setup log to string
        logSink = std::make_shared<spdlog::sinks::ostream_sink_mt>(logOss);
        logger = std::make_unique<spdlog::logger>("default", logSink);
        logger->set_level(spdlog::level::info);
        logger->set_pattern("%v");

        // export log functions to lua
        scripting::registerFunction(&luaLogInfo);
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
