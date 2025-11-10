#pragma once

#include "../../Native/MinecraftPackets.h"
#include "../../Native/RakNet/Packet.h"

#ifdef LIGHTUT_EXPORTS
#define RAKNET_API __declspec(dllexport)
#else
#define RAKNET_API __declspec(dllimport)
#endif

struct RAKNET_API RakNetSendToServer : public BaseEvent {
    EventID id() const override { return (EventID)EventIDUt::RakNetSendToServer; }

    void* LoopbackSender = nullptr;
    Packet* Packet = nullptr;

    bool cancel = false;
};
