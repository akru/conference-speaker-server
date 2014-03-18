#include "channel_information.h"
#include "cs_packet.h"

QJsonObject ChannelInformation::toJson() const
{
    QJsonObject obj;
    obj.insert("port", port);
    return obj;
}

ChannelInformation ChannelInformation::fromJson(const QJsonObject &json)
{
    QJsonValue chPort = json["port"];

    if (chPort.isUndefined())
        throw(BadPacket());

    return ChannelInformation((quint16)chPort.toDouble());
}
