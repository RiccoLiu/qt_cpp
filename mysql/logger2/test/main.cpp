

#include <iostream>
#include <spdlog/spdlog.h>

#include "logger2.h"
int main()
{
    std::cout << "[LOGGGER2 ] Hello World " << std::endl;

    // auto logger = spdlog::create<spdlog::sinks::windebug_sink_mt>("vs");
    // logger->info("Hello from spdlog in Visual Studio!");


    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    std::cout << "set level: debug " << std::endl;
    spdlog::set_level(spdlog::level::debug); // Set *global* log level to debug
    spdlog::debug("This message is debug message..");
    spdlog::trace("This message is trace message..");
    spdlog::info("This message is info message..");
    spdlog::info("This message is warn message..");


    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    int a = 5;
    double b = 3.14;

    spdlog::info("--- a = %d, b = %f ---", a, b);
    spdlog::info("--- a = {}, b = {} ---", a, b);

    LOGI("-----------LOGI TEST START----------\n");



    LOGI("--- a = %d, b = %f ---", a, b);
    LOGI("--- a = {}, b = {} ---", a, b);


    LOGI("-----------LOGI TEST EDN----------\n");


    // Compile time log levels
    // Note that this does not change the current log level, it will only
    // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

    // LOGI("--- a = %d, b = %f ---", a, b);
    // LOGI("--- a = {}, b = {} ---", a, b);
    return 0;
}
