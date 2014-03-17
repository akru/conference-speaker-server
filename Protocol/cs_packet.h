#ifndef CS_PACKET_H
#define CS_PACKET_H

#include <Qt>

// Bad packet exception
class BadPacket
        : public std::exception
{};

// Packet types
namespace PacketType
{
    const static quint8 DISCOVER           = 0x05;
    const static quint8 REGISTRATION       = 0x10;
    const static quint8 TRANSMIT           = 0x11;
    const static quint8 RESPONSE_SUCCESS   = 0x12;
    const static quint8 RESPONSE_FAILURE   = 0x13;
};

#endif // CS_PACKET_H
