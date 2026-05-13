#ifndef LIA_LOGS
#define LIA_LOGS
#include <string>
#include <stdarg.h>
#include <format>

#define LIA_VERSION_RUN 1
#define LIA_VERSION_DIST 2
#define LIA_VERSION_DEBUG 3

#define LIA_VERSION LIA_VERSION_DIST

#ifdef LIA_VERSION
    #if LIA_VERSION == LIA_VERSION_RUN
        #define LIA_LOG_LEVEL 3
    #endif
    #if LIA_VERSION == LIA_VERSION_DIST
        #define LIA_LOG_LEVEL 1
    #endif
    #if LIA_VERSION == LIA_VERSION_DEBUG
        #define LIA_LOG_LEVEL 5
    #endif
#endif

#ifndef LIA_LOG_LEVEL
    #define LIA_LOG_LEVEL 5
#endif

#define LIA_log(x, y) LIA::Logger::log(x, y, __FILE__, __LINE__, __FUNCTIONW__)
#define LIA_log_f(x, ...) LIA::Logger::log_f(x, __FILE__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#if LIA_LOG_LEVEL >= 0
    #define LIA_fatal(x) LIA::Logger::log(LIA::Logger::Level::FATAL, x, __FILE__, __LINE__, __FUNCTIONW__)
    #define LIA_fatal_f(...) LIA::Logger::log_f(LIA::Logger::Level::FATAL, __FILE__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#else
    #define LIA_fatal(x)
    #define LIA_fatal_f(x)
#endif
#if LIA_LOG_LEVEL >= 1
    #define LIA_error(x) LIA::Logger::log(LIA::Logger::Level::ERROR, x, __FILE__, __LINE__, __FUNCTIONW__)
    #define LIA_error_f(...) LIA::Logger::log_f(LIA::Logger::Level::ERROR, __FILE__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#else
    #define LIA_error(x)
    #define LIA_error_f(x)
#endif
#if LIA_LOG_LEVEL >= 2
    #define LIA_warn(x) LIA::Logger::log(LIA::Logger::Level::WARN, x, __FILE__, __LINE__, __FUNCTIONW__)
    #define LIA_warn_f(...) LIA::Logger::log_f(LIA::Logger::Level::WARN, __FILE__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#else
    #define LIA_warn(x)
    #define LIA_warn_f(x)
#endif
#if LIA_LOG_LEVEL >= 3
    #define LIA_info(x) LIA::Logger::log(LIA::Logger::Level::INFO, x, __FILE__, __LINE__, __FUNCTIONW__)
    #define LIA_info_f(...) LIA::Logger::log_f(LIA::Logger::Level::INFO, __FILE__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#else
    #define LIA_info(x)
    #define LIA_info_f(x)
#endif
#if LIA_LOG_LEVEL >= 4
    #define LIA_debug(x) LIA::Logger::log(LIA::Logger::Level::DEBUG, x, __FILE__, __LINE__, __FUNCTIONW__)
    #define LIA_debug_f(...) LIA::Logger::log_f(LIA::Logger::Level::DEBUG, __FILE__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#else
    #define LIA_debug(x)
    #define LIA_debug_f(x)
#endif
#if LIA_LOG_LEVEL >= 5
    #define LIA_trace(x) LIA::Logger::log(LIA::Logger::Level::TRACE, x, __FILE__, __LINE__, __FUNCTIONW__)
    #define LIA_trace_f(...) LIA::Logger::log_f(LIA::Logger::Level::TRACE, __FILE__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#else
    #define LIA_trace(x)
    #define LIA_trace_f(x)
#endif

#include <iostream>
#include <string_view>
#include <fstream>
namespace LIA
{
    static class Logger {
        public:
            static std::ofstream _file;
            static enum class Level {FATAL, ERROR, WARN, INFO, DEBUG, TRACE};
            // Level, msg, __FILE__, __LINE__, __FUNCTIONW_
            static void log(Level, std::string, std::string, int, std::wstring);
            template<typename... Args>
            static void log_f(Level level, std::string file, int line, std::wstring func, const std::string_view format, Args&&... args) {
                std::string msg = std::vformat(format, std::make_format_args(args...));
                log(level, msg, file, line, func);
            };
    };
} // namespace LIA
#endif