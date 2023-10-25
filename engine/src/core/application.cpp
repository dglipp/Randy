#include <core/application.h>
#include <core/logger.h>

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

    // TODO: remove
    R_FATAL("A test message");
    R_ERROR("A test message");
    R_WARN("A test message");
    R_INFO("A test message");
    R_DEBUG("A test message");
    R_TRACE("A test message");

    initialized = true;
}

void Application::run(){
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