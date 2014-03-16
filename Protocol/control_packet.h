#ifndef CONTROL_PACKET_H
#define CONTROL_PACKET_H

#include <QDataStream>

class BadPacket
        : public std::exception
{};

class ControlPacket
{
public:

    const static quint8 REGISTRATION       = 0x10;
    const static quint8 TRANSMIT           = 0x11;
    const static quint8 RESPONSE_SUCCESS   = 0x12;
    const static quint8 RESPONSE_FAILURE   = 0x13;

    // Constructor
    ControlPacket(quint8 t)
        : type(t)
    {}
    // Deserialization constructor
    ControlPacket(QDataStream &ds);
    // Packet serialization by data stream
    QByteArray serialize();

    inline quint8 getType() const
    {
        return type;
    }

private:
    quint8 type;
};


#endif // CONTROL_PACKET_H
