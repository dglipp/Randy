#pragma once

#include <cstdint>
#include <string>

#include "platform/platform.h"
#include "core/logger.h"
#include "game_types.h"
#include "core/event.h"

class Application{
    private:
        Game * gameInstance;

        int16_t startPosX;
        int16_t startPosY;
        int16_t startWidth;
        int16_t startHeight;
        std::string name;

        bool isRunning;
        bool isSuspended;
        Platform platform;
        int16_t width;
        int16_t height;
        double_t lastTime;
        const Logger logger;
        EventSystem eventSystem;

    public:
        Application(Game * gameInstance);

        void run();
};