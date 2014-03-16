#include "control_packet.h"

ControlPacket::ControlPacket(QDataStream &ds)
{
    // Restore custom values
    ds >> type;
}

QByteArray ControlPacket::serialize()
{
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << type;                             // Packet value
    return buffer;
}
