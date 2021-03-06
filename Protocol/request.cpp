#include "request.h"
#include "cs_packet.h"

QJsonObject Request::toJson() const
{
    QJsonObject obj;
    switch (type) {
    case Registration:
        obj.insert("request", QString("registration"));
        break;
    case ChannelOpen:
        obj.insert("request", QString("channel_open"));
        break;
    case ChannelClose:
        obj.insert("request", QString("channel_close"));
        break;
    case Vote:
        obj.insert("request", QString("vote"));
        break;
    }
    return obj;
}

Request Request::fromJson(const QJsonObject &json)
{
    QJsonValue type = json["request"];
    if (type.isUndefined())
        throw(BadPacket());

    if (type.toString() == "registration")
        return Request(Registration);
    if (type.toString() == "channel_open")
        return Request(ChannelOpen);
    if (type.toString() == "channel_close")
        return Request(ChannelClose);
    if (type.toString() == "vote")
        return Request(Vote);

    throw(BadPacket());
}
