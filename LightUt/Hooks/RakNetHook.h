#pragma once

void* __o__sendPacket = nullptr;
void SendPacketDetour(void* loopbackSender, Packet* packet)
{
    RakNetSendToServer dispatch;
    dispatch.Packet = packet;
    dispatch.LoopbackSender = loopbackSender;
    GameEvents::dispatch(&dispatch);

    if (dispatch.cancel)
        return;

    CallFunc<void, void*, void*>(__o__sendPacket, loopbackSender, packet);
}

void InitRakNetHook() {
    NativeCore::hookFunction(NativeCore::findSig("48 83 EC 28 48 ? ? 41 28 48 83 C0 01 74 27"), &SendPacketDetour, &__o__sendPacket);
    MinecraftPackets::CreatePacketPtr = NativeCore::findSig("48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 60 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 58 48 8B F9 48 89");
}
