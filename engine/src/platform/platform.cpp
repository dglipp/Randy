#include "platform/platform.h"
#include "core/logger.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

static double_t clockFrequency;
static LARGE_INTEGER startTime;

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

PlatformState::PlatformState(std::string applicationName, int32_t x, int32_t y, int32_t width, int32_t height){
    internalState = new InternalState;

    InternalState *state = (InternalState *)internalState;

    state->hInstance = GetModuleHandleA(0);

    // setup and register window class

    HICON icon = LoadIcon(state->hInstance, IDI_APPLICATION);
    WNDCLASSA wc = {0};

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->hInstance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = "randy_window_class";

    if(!RegisterClassA(&wc)){
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        throw("Can't register window");
    }

    uint32_t clientX = x;
    uint32_t clientY = y;
    uint32_t clientWidth = width;
    uint32_t clientHeight = height;

    uint32_t windowX = clientX;
    uint32_t windowY = clientY;
    uint32_t windowWidth = clientWidth;
    uint32_t windowHeight = clientHeight;

    uint32_t windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    long windowExStyle = WS_EX_APPWINDOW;

    windowStyle |= WS_MAXIMIZEBOX;
    windowStyle |= WS_MINIMIZEBOX;
    windowStyle |= WS_THICKFRAME;

    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

    windowX += borderRect.left;
    windowY += borderRect.top;

    windowWidth += borderRect.right;
    windowHeight += borderRect.bottom;

    HWND handle = CreateWindowExA(windowExStyle, "randy_window_class", applicationName.c_str(), windowStyle, windowX, windowY, windowWidth, windowHeight, 0, 0, state->hInstance, 0);

    if(handle == 0){
        MessageBoxA(nullptr, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        R_FATAL("Window creation failed!");
        throw("");
    } else {
        state->hwnd = handle;
    }

    bool shouldActivate = 1; // TODO: if the window should not accept input, this should be false.
    int32_t showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;

    ShowWindow(state->hwnd, showWindowCommandFlags);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clockFrequency = 1.0 / (double_t)frequency.QuadPart;
    QueryPerformanceCounter(&startTime);
}

PlatformState::~PlatformState(){
    InternalState *state = (InternalState *)internalState;

    if(state->hwnd){
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

bool PlatformState::pumpMessages(){
    MSG message;
    while(PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)){
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
}

    void *PlatformState::allocate(size_t size, bool aligned){
        return malloc(size);
    }

    void PlatformState::freeMemory(void *block, bool aligned){
        free(block);
    }

    void *PlatformState::zeroMemory(void *block, size_t size){
        return memset(block, 0, size);
    }

    void *PlatformState::copyMemory(void *dest, const void *source, size_t size){
        return memcpy(dest, source, size);
    }

    void *PlatformState::setMemory(void *dest, int32_t value, size_t size){
        return memset(dest, value, size);
    }
    	
    void PlatformState::consoleWrite(std::string message, uint8_t color){
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
        static uint8_t levels[6] = {64, 4, 6, 2, 1, 8};
        
        SetConsoleTextAttribute(consoleHandle, levels[color]);

        OutputDebugStringA(message.c_str());
        uint64_t length = message.length();
        LPDWORD numberWritten = 0;

        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message.c_str(), (DWORD)length, numberWritten, 0);
    }

    void PlatformState::consoleWriteError(std::string message, uint8_t color){
        HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);

        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
        static uint8_t levels[6] = {64, 4, 6, 2, 1, 8};
        
        SetConsoleTextAttribute(consoleHandle, levels[color]);

        OutputDebugStringA(message.c_str());
        uint64_t length = message.length();
        LPDWORD numberWritten = 0;

        WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message.c_str(), (DWORD)length, numberWritten, 0);
    }

    double_t PlatformState::getAbsoluteTime(){
        LARGE_INTEGER nowTime;
        QueryPerformanceCounter(&nowTime);
        return (double_t)nowTime.QuadPart * clockFrequency;
    }

    void PlatformState::sleep(uint64_t ms){
        Sleep((DWORD) ms);
    }

    LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam){
        switch (msg)
        {
        case WM_ERASEBKGND:
            return 1;

        case WM_CLOSE:
            // TODO: fire an event for the application to quit
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            // RECT r;
            // GetClientRect(hwnd, &r);
            // uint32_t width = r.right - r.left;
            // uint32_t height = r.bottom - r.top;
            // TODO: Fire an event for window resize
            break;
        
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            // bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: input processing
            break;
        
        case WM_MOUSEMOVE:
            // int32_t xPosition = GET_X_LPARAM(lParam);
            // int32_t yPosition = GET_Y_LPARAM(lParam);
            // TODO: input processing
            break;

        case WM_MOUSEWHEEL:
            // int32_t zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            // if(zDelta != 0){
            //     zDelta = (zDelta < 0) ? -1 : 1;
            // }
            // TODO: input processing
            break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            // bool pressed = msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN;
            // TODO: input processing
            break;
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

#endif