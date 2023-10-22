#include <core/logger.h>
#include <core/asserts.h>

void main(){
    const Logger logger;

    R_FATAL("A test message");
    R_ERROR("A test message");
    R_WARN("A test message");
    R_INFO("A test message");
    R_DEBUG("A test message");
    R_TRACE("A test message");

    R_ASSERT(false);
    return;
}