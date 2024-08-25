#include <core/logger.h>
#include <core/asserts.h>

// TODO: test
#include <platform/platform.h>

int main(void){
    RFATAL("A test message: %f", 3.14f);
    RERROR("A test message: %f", 3.14f);
    RWARN("A test message: %f", 3.14f);
    RINFO("A test message: %f", 3.14f);
    RDEBUG("A test message: %f", 3.14f);
    RTRACE("A test message: %f", 3.14f);

    platform_state state;

    if(platform_startup(&state, "Randy engine testbed", 100, 100, 1200, 720)){
        while (TRUE)
        {
            platform_pump_messages(&state);
        }
    }

    platform_shutdown(&state);
    return 0;
}
