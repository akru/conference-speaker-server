#ifndef CS_PACKET_H
#define CS_PACKET_H

#include <Qt>

// Bad packet exception
class BadPacket
        : public std::exception
{};

static const quint16 SERVER_DISCOVER_PORT    = 35000;
static const quint16 SERVER_CONNECTION_PORT  = 35001;
static const quint16 SERVER_APP_PORT         = 35080;

#endif // CS_PACKET_H
