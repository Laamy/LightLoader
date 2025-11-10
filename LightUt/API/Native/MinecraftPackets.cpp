#include "MinecraftPackets.h"

template <typename R, typename... Args>
R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }

// dont actually use this/write ur own
std::shared_ptr<void*> MinecraftPackets::_CreatePacket(PacketId id)
{
    return CallFunc<std::shared_ptr<void*>, PacketId>((void*)CreatePacketPtr, id);
}
