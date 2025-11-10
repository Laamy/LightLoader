#pragma once

#include <chrono>

enum class PacketPriority : int {
    ImmediatePriority = 0,
    HighPriority = 1,
    MediumPriority = 2,
    LowPriority = 3,
    NumberOfPriorities = 4,
};

namespace NetworkPeer {
    enum class Reliability : int {
        Reliable = 0,
        ReliableOrdered = 1,
        Unreliable = 2,
        UnreliableSequenced = 3,
    };
}

typedef unsigned char uchar; // bro its literally a fucking uint8_t
enum class SubClientId : uchar {
    Server = 0,
    PrimaryClient = 0,
    Client2 = 1,
    Client3 = 2,
    Client4 = 3,
    ExtraIdSlotStart = 100,
    EditorUI = 101,
};

enum class Compressibility : int {
    Compressible = 0,
    Incompressible = 1,
};

typedef unsigned short ushort;
namespace NetherNet { struct NetworkID {}; }
namespace RakNet {
    struct RakNetGUID {
    public:
        uint64_t g;
        ushort systemIndex;
    };
}

class NetworkIdentifier {
public:
    enum class Type : int {
        RakNet = 0,
        Address = 1,
        Address6 = 2,
        NetherNet = 3,
        Invalid = 4,
    };

public:
    NetherNet::NetworkID        mNetherNetId;
    RakNet::RakNetGUID          mGuid;
    void*                       mSock;
    NetworkIdentifier::Type     mType;
};

class IPacketHandlerDispatcher;

class BinaryStream;
class ReadOnlyBinaryStream;
class SerializationMode;
namespace cereal { class ReflectionCtx; }

typedef uint64_t uint64;
class Packet {
public:
    PacketPriority                          mPriority;
    NetworkPeer::Reliability                mReliability;
    SubClientId                             mSenderSubId;
    bool                                    mIsHandled;
    std::chrono::steady_clock::time_point   mReceiveTimepoint;
    IPacketHandlerDispatcher const*         mHandler;
    Compressibility                         mCompressible;

public:
    virtual ~Packet();
    virtual PacketId getId() const = 0;
    virtual ::std::string getName() const = 0;
    virtual uint64 getMaxSize() const;
    virtual void* checkSize(uint64 packetSize, bool receiverIsServer) const;
    virtual void writeWithSerializationMode(BinaryStream& bitStream, ::cereal::ReflectionCtx const& reflectionCtx, std::optional<SerializationMode>) const;
    virtual void write(BinaryStream& bitStream, cereal::ReflectionCtx const& reflectionCtx) const;
    virtual void write(BinaryStream&) const = 0;
    virtual void* read(ReadOnlyBinaryStream& bitStream, ::cereal::ReflectionCtx const& reflectionCtx);
    virtual void* read(ReadOnlyBinaryStream& bitStream);
    virtual bool disallowBatching() const;
    virtual bool isValid() const;
    virtual SerializationMode getSerializationMode() const;
    virtual void setSerializationMode(::SerializationMode);
    virtual std::string toString() const;
    virtual void* _read(ReadOnlyBinaryStream& bitStream, ::cereal::ReflectionCtx const& reflectionCtx);
    virtual void* _read(ReadOnlyBinaryStream&) = 0;
};

class IPacketHandlerDispatcher {
public:
    virtual ~IPacketHandlerDispatcher() = default;
    virtual void handle(NetworkIdentifier const&, void*, std::shared_ptr<Packet>&) const = 0;
};