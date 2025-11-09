#pragma once

enum class EventID : uint32_t {
    WndProc,
    Loaded,
    Closing,
    EVENT_COUNT
};

struct GAMEEVENTS_API BaseEvent {
    virtual ~BaseEvent() = default;
    virtual EventID id() const = 0;
};

struct GAMEEVENTS_API ListenerRecord {
    size_t token;
    std::function<void(BaseEvent*)> callback;
};
