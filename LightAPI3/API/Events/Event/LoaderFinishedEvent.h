#pragma once

struct GAMEEVENTS_API LoaderFinishedEvent : public BaseEvent {
    EventID id() const override { return EventID::LoaderFinal; }
};
