#ifndef CS_PACKET_H
#define CS_PACKET_H

#include <Qt>

// Bad packet exception
class BadPacket
        : public std::exception
{};

const quint16 SERVER_INFORMATION_PORT = 35000;
const quint16 SERVER_CONNECTION_PORT  = 35001;

#endif // CS_PACKET_H
