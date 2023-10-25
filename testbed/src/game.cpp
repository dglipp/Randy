#include "game.h"
#include <core/logger.h>

bool gameInitialize(Game *game){
    R_DEBUG("gameInitialize() called");
    return true;
}

bool gameUpdate(Game *game, float_t deltaTime){

    return true;
}

bool gameRender(Game *game, float_t deltaTime){
    return true;
}

bool gameOnResize(Game *game, uint32_t width, uint32_t height){
    return true;
}