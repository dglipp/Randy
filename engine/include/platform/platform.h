#pragma once

#include <string>
#include <cstdint>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>
#include <windowsx.h>
struct InternalState{
    HINSTANCE hInstance;
    HWND hwnd;
};

#endif

class PlatformState
{
    void *internalState;


    void *allocate(size_t size, bool aligned);
    void freeMemory(void *block, bool aligned);
    void *zeroMemory(void *block, size_t size);
    void *copyMemory(void *dest, const void *source, size_t size);
    void *setMemory(void *dest, int32_t value, size_t size);
    double_t getAbsoluteTime();
    void sleep(uint64_t ms);

    public:
        PlatformState(std::string applicationName, int32_t x, int32_t y, int32_t width, int32_t height);
        ~PlatformState();
        static void consoleWrite(std::string message, uint8_t color);
        static void consoleWriteError(std::string message, uint8_t color);
        bool pumpMessages();
};