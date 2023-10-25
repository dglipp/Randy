#pragma once

#include <string>

struct Game {
    int16_t startPosX;
    int16_t startPosY;
    int16_t startWidth;
    int16_t startHeight;
    std::string name;

    bool (*initialize)(struct Game *game);
    bool (*update)(struct Game *game, float_t deltaTime);
    bool (*render)(struct Game *game, float_t deltaTime);
    bool (*onResize)(struct Game *game, uint32_t width, uint32_t height);

    void *state;
};