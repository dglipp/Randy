#include "core/asserts.h"
#include "core/logger.h"

void report_assertion_failure(std::string expression, std::string message, std::string file, int line){

    log_output(LOG_LEVEL_FATAL, "Assertion Failure: " + expression + ", message: " + message + ", in file: " + file + ", in line: " + std::to_string(line) + "\n");
}