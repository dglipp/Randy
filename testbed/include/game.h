#pragma once

#include <game_types.h>

struct GameState{
    float_t deltaTime;
};

bool gameInitialize(Game *game);
bool gameUpdate(Game *game, float_t deltaTime);
bool gameRender(Game *game, float_t deltaTime);
bool gameOnResize(Game *game, uint32_t width, uint32_t height);