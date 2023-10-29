#include "core/logger.h"
#include "platform/platform.h"

// TODO: temporary
#include <iostream>


void log_output(log_level level, std::string message){
    bool isError = level < LOG_LEVEL_WARN;

    if(isError){
        Platform::consoleWriteError(Logger::level_strings[level] + message + "\n", level);
    } else{
        Platform::consoleWrite(Logger::level_strings[level] + message + "\n", level);
    }
}

Logger::Logger(){
    // TODO: create log file
}

Logger::~Logger(){
    // TODO: cleanup logging/write queued entries
}

const std::string Logger::level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
