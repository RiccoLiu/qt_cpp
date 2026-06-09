
#include "Logger2.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>
#include <mutex>
#include <cstdarg>
#include <string>
#include <vector>

namespace {

std::shared_ptr<spdlog::logger> g_logger;
std::once_flag g_init_flag;

spdlog::level::level_enum ToSpdLogLevel(Logger2::Level level) {
    switch (level) {
    case Logger2::Level::Trace:    return spdlog::level::trace;
    case Logger2::Level::Debug:    return spdlog::level::debug;
    case Logger2::Level::Info:     return spdlog::level::info;
    case Logger2::Level::Warn:     return spdlog::level::warn;
    case Logger2::Level::Error:    return spdlog::level::err;
    case Logger2::Level::Critical: return spdlog::level::critical;
    default: return spdlog::level::info;
    }
}

void EnsureInitialized() {
    std::call_once(g_init_flag, []() {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        std::vector<spdlog::sink_ptr> sinks{console_sink};
        g_logger = std::make_shared<spdlog::logger>("Logger2", sinks.begin(), sinks.end());
        spdlog::register_logger(g_logger);
        g_logger->set_level(spdlog::level::info);
        g_logger->flush_on(spdlog::level::warn);
    });
}

std::string FormatVa(const char* fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int len = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (len <= 0)
        return {};

    std::vector<char> buf(len + 1);
    std::vsnprintf(buf.data(), buf.size(), fmt, args);
    return std::string(buf.data(), len);
}

void LogImpl(spdlog::level::level_enum lvl, const char* fmt, va_list args) {
    if (!g_logger) EnsureInitialized();
    std::string msg = FormatVa(fmt, args);
    g_logger->log(lvl, "{}", msg); // spdlog 会再格式化一次，但 msg 已是字符串
    // 或者直接：g_logger->log(lvl, fmt, ...); 但需要模板，这里简化
}

} // anonymous namespace

namespace Logger2 {

void Init(Level level, const char* logFile) {
    std::call_once(g_init_flag, [level, logFile]() {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        if (logFile && logFile[0]) {
            sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true));
        }

        for (auto& sink : sinks) {
            sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        }

        g_logger = std::make_shared<spdlog::logger>("Logger2", sinks.begin(), sinks.end());
        spdlog::register_logger(g_logger);
        g_logger->set_level(ToSpdLogLevel(level));
        g_logger->flush_on(spdlog::level::warn);
    });
}

void SetLevel(Level level) {
    if (g_logger) {
        g_logger->set_level(ToSpdLogLevel(level));
    }
}

#define DEFINE_LOG_FUNC(NAME, LEVEL) \
void NAME(const char* fmt, ...) { \
        va_list args; \
        va_start(args, fmt); \
        LogImpl(LEVEL, fmt, args); \
        va_end(args); \
}

DEFINE_LOG_FUNC(LogTrace,    spdlog::level::trace)
DEFINE_LOG_FUNC(LogDebug,    spdlog::level::debug)
DEFINE_LOG_FUNC(LogInfo,     spdlog::level::info)
DEFINE_LOG_FUNC(LogWarn,     spdlog::level::warn)
DEFINE_LOG_FUNC(LogError,    spdlog::level::err)
DEFINE_LOG_FUNC(LogCritical, spdlog::level::critical)

} // namespace Logger2
