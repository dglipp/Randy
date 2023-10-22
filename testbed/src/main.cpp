#include <core/logger.h>
#include <core/asserts.h>

//TODO: Test
#include <platform/platform.h>

void main(){
    const Logger logger;

    R_FATAL("A test message");
    R_ERROR("A test message");
    R_WARN("A test message");
    R_INFO("A test message");
    R_DEBUG("A test message");
    R_TRACE("A test message");

    PlatformState state("Randy Engine Testbed", 100, 100, 1280, 720);

    while(true){
        state.pumpMessages();
    }

    return;
}