#include "channel_information.h"
#include "cs_packet.h"

QJsonObject ChannelInformation::toJson() const
{
    QJsonObject obj;
    obj.insert("host", host);
    obj.insert("port", port);
    return obj;
}

ChannelInformation ChannelInformation::fromJson(const QJsonObject &json)
{
    QJsonValue chHost = json["host"];
    QJsonValue chPort = json["port"];

    if (chHost.isUndefined() || chPort.isUndefined())
        throw(BadPacket());

    return ChannelInformation(chHost.toString(), (quint16)chPort.toDouble());
}
