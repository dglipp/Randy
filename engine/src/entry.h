#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

extern b8 create_game(game* game);

// main entry point
int main(void){
    game game_instance;
    if(!create_game(&game_instance)){
        RFATAL("Could not create game");

        return -1;
    }

    if(!game_instance.render || !game_instance.update || !game_instance.initialize || !game_instance.on_resize){
        RFATAL("Game's function pointers not defined");
        return -2;
    }

    // init
    if(!application_create(&game_instance)){
        RINFO("Application failed to create");
    }

    // game loop
    if(!application_run()){
        RINFO("Application did not shutdown gracefully");
        return 2;
    }

    return 0;
}