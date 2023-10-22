#include "core/logger.h"

// TODO: temporary
#include <iostream>


void log_output(log_level level, std::string message){
    bool is_error = level < 2;

    // TODO: plan specific output
    std::cout << Logger::level_strings[level] <<  message << "\n";
}

Logger::Logger(){
    // TODO: create log file
}

Logger::~Logger(){
    // TODO: cleanup logging/write queued entries
}

const std::string Logger::level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
