#pragma once

#include <cstdint>
#include <vector>
#include <map>

typedef struct EventContext {
    union {
        int64_t i64[2];
        uint64_t u64[2];
        double f64[2];

        int32_t i32[4];
        uint32_t u32[4];
        float f32[4];

        int16_t i16[8];
        uint16_t u16[8];

        int8_t i8[16];
        uint8_t u8[16];

        char c[16];
    } data;
} EventContext;

typedef enum SystemEventCode {
    EVENT_CODE_APPLICATION_QUIT = 0x01,
    EVENT_CODE_KEY_PRESSED = 0x02,
    EVENT_CODE_KEY_RELEASED = 0x03,
    EVENT_CODE_BUTTON_PRESSED = 0x04,
    EVENT_CODE_BUTTON_RELEASED = 0x05,
    EVENT_CODE_MOUSE_MOVED = 0x06,
    EVENT_CODE_MOUSE_WHEEL = 0x07,
    EVENT_CODE_RESIZED = 0x08,
    EVENT_CODE_SET_RENDER_MODE = 0x0A,
    EVENT_CODE_DEBUG0 = 0x10,
    EVENT_CODE_DEBUG1 = 0x11,
    EVENT_CODE_DEBUG2 = 0x12,
    EVENT_CODE_DEBUG3 = 0x13,
    EVENT_CODE_DEBUG4 = 0x14,
    EVENT_CODE_OBJECT_HOVER_ID_CHANGED = 0x15,
    EVENT_CODE_DEFAULT_RENDERTARGET_REFRESH_REQUIRED = 0x16,
    EVENT_CODE_KVAR_CHANGED = 0x17,
    EVENT_CODE_WATCHED_FILE_WRITTEN = 0x18,
    EVENT_CODE_WATCHED_FILE_DELETED = 0x19,
    EVENT_CODE_MOUSE_DRAGGED = 0x20,
    EVENT_CODE_MOUSE_DRAG_BEGIN = 0x21,
    EVENT_CODE_MOUSE_DRAG_END = 0x22,
    MAX_EVENT_CODE = 0xFF
} SystemEventCode;

typedef bool (* PFN_onEvent)(uint16_t code, void *sender, void *listenerInstance, EventContext data);

typedef struct RegisteredEvent {
    void *listener;
    PFN_onEvent callback;
} RegisteredEvent;

class EventSystem {
    std::map<uint16_t, std::vector<RegisteredEvent>> events;

public:
    EventSystem();
    ~EventSystem();

    bool eventRegister(uint16_t code, void *listener, PFN_onEvent onEvent);
    bool eventUnregister(uint16_t code, void *listener, PFN_onEvent onEvent);
    bool eventFire(uint16_t code, void *sender, EventContext context);
};

