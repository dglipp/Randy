#include <entry.h>
#include "game.h"

// TODO: remove that
#include <platform/platform.h>

b8 create_game(game * game_instance){

    game_instance->app_config.start_pos_x = 100;
    game_instance->app_config.start_pos_y = 100;
    game_instance->app_config.start_pos_width = 1200;
    game_instance->app_config.start_pos_height = 720;
    game_instance->app_config.name = "Randy engine testbed";
    game_instance->render = game_render;
    game_instance->initialize = game_initialize;
    game_instance->on_resize = game_on_resize;
    game_instance->update = game_update;

    game_instance->state = platform_allocate(sizeof(game_state), FALSE);

    return TRUE;
}