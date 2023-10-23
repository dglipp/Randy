#include <core/application.h>
#include <core/logger.h>

static bool initialized = false;

Application::Application(std::string name, int16_t startPosX, int16_t startPosY, int16_t startWidth, int16_t startHeight)
    : startPosX(startPosX)
    , startPosY(startPosY)
    , startWidth(startWidth)
    , startHeight(startHeight)
    , name(name)
    , isRunning(true)
    , isSuspended(false)
    , platform(name, startPosX, startPosY, startWidth, startHeight)
    , logger()
{
    if (initialized)
    {
        R_ERROR("Application called more than once!");
        throw("");
    }

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
    }

    this->isRunning = false;
}