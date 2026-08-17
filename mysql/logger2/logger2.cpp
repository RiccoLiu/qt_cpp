// Logger2.cpp
#include "Logger2.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>
#include <mutex>
#include <cstdarg>
#include <string>
#include <vector>

#ifdef VS_MSVC_PRINT
#include <windows.h>
#endif

namespace spdlog {
namespace sinks {

#ifdef VS_MSVC_PRINT

template<typename Mutex>
class msvc_sink : public base_sink<Mutex>
{
public:
    explicit msvc_sink() = default;

protected:
    void sink_it_(const details::log_msg& msg) override
    {
        memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        formatted.push_back('\0'); // null-terminate for OutputDebugString

        // OutputDebugStringA 接收 ANSI 字符串
        ::OutputDebugStringA(fmt::to_string(formatted).c_str());
    }

    void flush_() override {}
};

using msvc_sink_mt = msvc_sink<std::mutex>;
using msvc_sink_st = msvc_sink<details::null_mutex>;

#endif

} // namespace sinks
} // namespace spdlog

namespace {

std::shared_ptr<spdlog::logger> g_logger;
std::mutex g_logger_mutex; // 用于线程安全地修改 sinks

void console_only_pattern(const std::shared_ptr<spdlog::sinks::sink>& sink) {
    sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
}

// 初始化基础 console logger（延迟初始化，线程安全）
void EnsureConsoleLogger() {
    std::lock_guard<std::mutex> lock(g_logger_mutex);
    if (g_logger)
        return;

    std::vector<spdlog::sink_ptr> sinks;

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_only_pattern(console_sink); // 单独定义 pattern
    sinks.push_back(console_sink);

#ifdef VS_MSVC_PRINT
    auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    console_only_pattern(msvc_sink); // 单独定义 pattern
    sinks.push_back(msvc_sink);
#endif

    g_logger = std::make_shared<spdlog::logger>("Logger", sinks.begin(), sinks.end());
    spdlog::register_logger(g_logger);
    g_logger->set_level(spdlog::level::trace); // 允许所有级别，由 Init 控制最终 level
    g_logger->flush_on(spdlog::level::info);
}

void full_pattern(const std::shared_ptr<spdlog::sinks::sink>& sink) {
    sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
}

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

std::string FormatVa(const char* fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int len = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (len <= 0) return {};

    std::vector<char> buf(len + 1);
    std::vsnprintf(buf.data(), buf.size(), fmt, args);
    return std::string(buf.data(), len);
}

void LogImpl(spdlog::level::level_enum lvl, const char* fmt, va_list args) {
    EnsureConsoleLogger(); // 确保至少有 console logger
    std::string msg = FormatVa(fmt, args);
    g_logger->log(lvl, "{}", msg);
}

} // anonymous namespace

namespace Logger2 {

void Init(Level level, const char* logFile) {
    EnsureConsoleLogger(); // 确保 logger 已存在

    std::lock_guard<std::mutex> lock(g_logger_mutex);

    // 如果已经添加过 file sink，可选择跳过或替换（这里允许多次调用，但只加一次 file）
    static bool file_sink_added = false;
    if (logFile && logFile[0] && !file_sink_added) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
        full_pattern(file_sink);
        g_logger->sinks().push_back(file_sink);
        file_sink_added = true;
    }

    // 更新日志级别
    g_logger->set_level(ToSpdLogLevel(level));
}

void SetLevel(Level level) {
    if (!g_logger) EnsureConsoleLogger();
    g_logger->set_level(ToSpdLogLevel(level));
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
