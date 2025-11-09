#pragma once

struct GAMEEVENTS_API LoadEvent : public BaseEvent {
    EventID id() const override { return EventID::Loaded; }
};
