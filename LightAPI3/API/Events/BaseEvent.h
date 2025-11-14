#pragma once

enum class EventID : uint32_t {
    WndProc,
    D3DRender,
    Loaded,
    Closing,
    EVENT_COUNT
};

//#define EventID_WndProc 0
//#define EventID_Loaded 1
//#define EventID_Closing 2

struct GAMEEVENTS_API BaseEvent {
    virtual ~BaseEvent() = default;
    virtual EventID id() const = 0;
};

struct GAMEEVENTS_API ListenerRecord {
    size_t token;
    std::function<void(BaseEvent*)> callback;
};
