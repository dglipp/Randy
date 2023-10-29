#include <entry.h>
#include "game.h"
#include <core/rmemory.h>

bool createGame(Game * game){
    game->startPosX = 100;
    game->startPosY = 100;
    game->startWidth = 1280;
    game->startHeight = 720;
    game->name = "Randy Engine Testbed";

    game->update = gameUpdate;
    game->render = gameRender;
    game->onResize = gameOnResize;
    game->initialize = gameInitialize;

    game->state = MemoryInterface::allocate(sizeof(GameState), MEMORY_TAG_GAME);

    return true;
}