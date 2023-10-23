#pragma once

#include <cstdint>
#include <string>

#include <platform/platform.h>
#include <core/logger.h>

class Application{
    private:
        int16_t startPosX;
        int16_t startPosY;
        int16_t startWidth;
        int16_t startHeight;
        std::string name;

        bool isRunning;
        bool isSuspended;
        PlatformState platform;
        int16_t width;
        int16_t height;
        double_t lastTime;
        const Logger logger;

    public:
        Application(std::string name, int16_t startPosX, int16_t startPosY, int16_t startWidth, int16_t startHeight);

        void run();
};