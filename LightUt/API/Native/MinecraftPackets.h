#pragma once

#include <windows.h>
#include <memory>

#ifdef LIGHT_EXPORTS
#define GAMEEVENTSUT_API __declspec(dllexport)
#else
#define GAMEEVENTSUT_API __declspec(dllimport)
#endif

#include "PacketId.h"

namespace MinecraftPackets
{
    static uintptr_t CreatePacketPtr;

    // dont actually use this/write ur own
    std::shared_ptr<void*> _CreatePacket(PacketId id);
}
