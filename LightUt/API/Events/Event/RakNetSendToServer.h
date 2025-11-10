#pragma once

struct GAMEEVENTSUT_API RakNetSendToServer : public BaseEvent {
    EventID id() const override { return (EventID)EventIDUt::RakNetSendToServer; }

    void* Packet;
};
