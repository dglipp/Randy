#include "core/application.h"
#include "core/logger.h"
#include "core/rmemory.h"

static bool initialized = false;

Application::Application(Game * gameInstance)
    : startPosX(gameInstance->startPosX)
    , startPosY(gameInstance->startPosY)
    , startWidth(gameInstance->startWidth)
    , startHeight(gameInstance->startHeight)
    , width(gameInstance->startWidth)
    , height(gameInstance->startHeight)
    , name(gameInstance->name)
    , gameInstance(gameInstance)
    , isRunning(true)
    , isSuspended(false)
    , platform(gameInstance->name, gameInstance->startPosX, gameInstance->startPosY, gameInstance->startWidth, gameInstance->startHeight)
    , logger()
    , eventSystem()
{
    if (initialized)
    {
        R_ERROR("Application called more than once!");
        throw("");
    }

    if(!gameInstance->initialize(gameInstance)){
        R_FATAL("Game failed to initialize!");
        throw("");
    }

    gameInstance->onResize(gameInstance, width, height);

    initialized = true;
}

void Application::run(){
    R_INFO(MemoryInterface::getMemoryUsageString());

    while(this->isRunning){
        if(!platform.pumpMessages())
        {
            this->isRunning = false;
        }

        if(!isSuspended){
            
            if(!this->gameInstance->update(this->gameInstance, 0.0f)) {
                R_FATAL("Game update failed, shutting down.");
                this->isRunning = false;
                break;
            }
            if(!this->gameInstance->render(this->gameInstance, 0.0f)) {
                R_FATAL("Game render failed, shutting down.");
                this->isRunning = false;
                break;
            }
        }
    }
    this->isRunning = false;
}