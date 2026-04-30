
#include "easylogging++.h"

#include "logger.h"
#include <string>

INITIALIZE_EASYLOGGINGPP

static struct EasyloggingInit {
    EasyloggingInit() {
        el::Loggers::addFlag(el::LoggingFlag::CreateLoggerAutomatically);
    }
} s_init;

static el::Level toEasyLoggingLevel(int level) {
    switch (level)
    {
    case LOG_LEVEL_TRACE:   return el::Level::Trace;
    case LOG_LEVEL_DEBUG:   return el::Level::Debug;
    case LOG_LEVEL_INFO:    return el::Level::Info;
    case LOG_LEVEL_WARNING: return el::Level::Warning;
    case LOG_LEVEL_ERROR:   return el::Level::Error;
    case LOG_LEVEL_FATAL:   return el::Level::Fatal;
    default:                return el::Level::Info;
    }
}

extern "C" {

void logger_enable_file_logging(const char* filename, unsigned int max_file_size_mb, int max_files) {
    el::Configurations conf;
    conf.setToDefault();
    conf.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
    conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
    conf.set(el::Level::Global, el::ConfigurationType::Filename, filename);
    conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, std::to_string(max_file_size_mb * 1024 * 1024));
    // conf.set(el::Level::Global, el::ConfigurationType::MaxLogFilesCount, std::to_string(max_files));
    el::Loggers::reconfigureAllLoggers(conf);
}

void logger_flush() {
    el::Loggers::getLogger("")->flush();
}

void logger_log(int level, const char* format, ...) {
    if (!el::Loggers::hasLogger("default")) {
        return;
    }

    // 格式化消息
    char buffer[2048];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 截断保护
    if (len < 0) len = 0;
    if (len >= (int)sizeof(buffer)) len = sizeof(buffer) - 1;
    buffer[len] = '\0';

    switch (toEasyLoggingLevel(level)) {
    case el::Level::Fatal:
        CLOG(FATAL, "default") << buffer;
        break;
    case el::Level::Error:
        CLOG(ERROR, "default") << buffer;
        break;
    case el::Level::Warning:
        CLOG(WARNING, "default") << buffer;
        break;
    case el::Level::Info:
        CLOG(INFO, "default") << buffer;
        break;
    case el::Level::Debug:
        CLOG(DEBUG, "default") << buffer;
        break;
    case el::Level::Trace:
        CLOG(TRACE, "default") << buffer;
        break;
    default:
        CLOG(INFO, "default") << buffer;
        break;
    }
}

} // extern "C"
