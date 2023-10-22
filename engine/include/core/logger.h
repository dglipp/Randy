#pragma once

#include <string>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1


#if R_RELEASE == 0
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1
#else
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

class Logger{

    public:
        Logger();
        ~Logger();

        static const std::string level_strings[6];
};

void log_output(log_level level, std::string message);

#define R_FATAL(message) log_output(LOG_LEVEL_FATAL, message);
#define R_ERROR(message) log_output(LOG_LEVEL_ERROR, message);

#if LOG_WARN_ENABLED == 1
    #define R_WARN(message) log_output(LOG_LEVEL_WARN, message);
#else
    #define R_WARN(message)
#endif

#if LOG_INFO_ENABLED == 1
    #define R_INFO(message) log_output(LOG_LEVEL_INFO, message);
#else
    #define R_INFO(message)
#endif

#if LOG_DEBUG_ENABLED == 1
    #define R_DEBUG(message) log_output(LOG_LEVEL_DEBUG, message);
#else
    #define R_DEBUG(message)
#endif

#if LOG_TRACE_ENABLED == 1
    #define R_TRACE(message) log_output(LOG_LEVEL_TRACE, message);
#else
    #define R_TRACE(message)
#endif


