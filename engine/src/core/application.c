#include "application.h"
#include "platform/platform.h"
#include "logger.h"
#include "game_types.h"


typedef struct application_state {
    game* game_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
} application_state;

static b8 initialized = FALSE;
static application_state app_state;

b8 application_create(game * game_instance){
    if(initialized) {
        RERROR("Application create called more than once");
        return FALSE;
    }

    app_state.game_instance = game_instance;
    
    // initlalize subsystems
    initialize_logging();
    // TODO: remove
    RFATAL("A test message: %f", 3.14f);
    RERROR("A test message: %f", 3.14f);
    RWARN("A test message: %f", 3.14f);
    RINFO("A test message: %f", 3.14f);
    RDEBUG("A test message: %f", 3.14f);
    RTRACE("A test message: %f", 3.14f);

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if(!platform_startup(&app_state.platform,
        game_instance->app_config.name, 
        game_instance->app_config.start_pos_x,
        game_instance->app_config.start_pos_y,
        game_instance->app_config.start_pos_width,
        game_instance->app_config.start_pos_height)){
        return FALSE;
    }

    if(!app_state.game_instance->initialize(app_state.game_instance)){
        RFATAL("Game failed o initialize");
        return FALSE;
    }

    app_state.game_instance->on_resize(app_state.game_instance, app_state.width, app_state.height);
    initialized = TRUE;
    return TRUE;
}

b8 application_run(){
    while (app_state.is_running)
    {
        if(!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended){
            if(!app_state.game_instance->update(app_state.game_instance, (f32) 0)){
                RFATAL("Game update failed");
                app_state.is_running = FALSE;
            }
        }
    }

    app_state.is_running = FALSE;
    platform_shutdown(&app_state.platform);

    return TRUE;
}