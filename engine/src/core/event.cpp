#include "core/event.h"
#include "core/rmemory.h"
#include "core/logger.h"

EventSystem::EventSystem()
    : events()
{
}

EventSystem::~EventSystem() {}

bool EventSystem::eventRegister(uint16_t code, void *listener, PFN_onEvent onEvent)
{
    for (auto el : this->events[code])
    {
        if (el.listener == listener)
            return false;
    }

    RegisteredEvent event;
    event.callback = onEvent;
    event.listener = listener;

    this->events[code].push_back(event);
    return true;
}

bool EventSystem::eventUnregister(uint16_t code, void *listener, PFN_onEvent onEvent)
{
    if (this->events.find(code) == this->events.end())
        return false;

    auto it = std::remove_if(this->events[code].begin(), this->events[code].end(), [listener, onEvent](RegisteredEvent element)
                             { return (element.callback == onEvent && element.listener == listener); });

    if (it != this->events[code].end())
    {
        this->events[code].erase(it, this->events[code].end());
        return true;
    }
    else
        return false;
}

bool EventSystem::eventFire(uint16_t code, void *sender, EventContext context)
{
    for (auto el : this->events[code])
    {
        if (el.callback(code, sender, el.listener, context))
        {
            return true;
        }
    }
    return false;
}