#pragma once

struct GAMEEVENTS_API ClosingEvent : public BaseEvent {
    EventID id() const override { return EventID::Closing; }
};
