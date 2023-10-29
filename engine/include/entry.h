#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "core/rmemory.h"
#include "game_types.h"

extern bool createGame(Game *game);

int main(){

    MemoryInterface memorySubsystem;
    Game gameInstance;


    if(!createGame(&gameInstance)){
        R_FATAL("Could not create game");
        return -1;
    }

    if(!gameInstance.render || !gameInstance.initialize || !gameInstance.onResize || !gameInstance.update){
        R_FATAL("The game function pointers must be assigned!");
        return -2;
    }
    
    Application app(&gameInstance);
    app.run();
}