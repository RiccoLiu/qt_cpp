#ifndef LOGGER2_H
#define LOGGER2_H


#include "logger2_global.h"

namespace Logger2 {

enum class Level {
    Trace, Debug, Info, Warn, Error, Critical
};

LOGGER2_EXPORT void Init(Level level = Level::Info, const char* logFile = nullptr);
LOGGER2_EXPORT void SetLevel(Level level);

LOGGER2_EXPORT void LogTrace(const char* fmt, ...);
LOGGER2_EXPORT void LogDebug(const char* fmt, ...);
LOGGER2_EXPORT void LogInfo(const char* fmt, ...);
LOGGER2_EXPORT void LogWarn(const char* fmt, ...);
LOGGER2_EXPORT void LogError(const char* fmt, ...);
LOGGER2_EXPORT void LogCritical(const char* fmt, ...);

} // namespace Logger2

// 宏封装（可选，但注意：宏内部调用的是 DLL 内部函数，不暴露 shared_ptr）
#define LOGT(fmt, ...) Logger2::LogTrace(fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) Logger2::LogDebug(fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) Logger2::LogInfo(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) Logger2::LogWarn(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) Logger2::LogError(fmt, ##__VA_ARGS__)
#define LOGC(fmt, ...) Logger2::LogCritical(fmt, ##__VA_ARGS__)

/*

    Logger2::Init();


*/

#endif // LOGGER2_H
