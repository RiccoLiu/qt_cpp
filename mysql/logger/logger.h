#ifndef LOGGER_H
#define LOGGER_H

#include "logger_global.h"

typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LoggerLevel;

extern "C" {

LOGGER_EXPORT void logger_enable_file_logging(const char* filename,
                                              unsigned int max_file_size_mb,
                                              int max_files);

LOGGER_EXPORT void logger_flush();
LOGGER_EXPORT void logger_log(int level, const char* format, ...);

}

// #define LOGT(...) logger_log(LOG_LEVEL_TRACE, __VA_ARGS__)
// #define LOGD(...) logger_log(LOG_LEVEL_DEBUG, __VA_ARGS__)
// #define LOGI(...) logger_log(LOG_LEVEL_INFO,  __VA_ARGS__)
// #define LOGW(...) logger_log(LOG_LEVEL_WARNING, __VA_ARGS__)
// #define LOGE(...) logger_log(LOG_LEVEL_ERROR, __VA_ARGS__)
// #define LOGF(...) logger_log(LOG_LEVEL_FATAL, __VA_ARGS__)


#define LOGT(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGF(...)


#endif // LOGGER_H
