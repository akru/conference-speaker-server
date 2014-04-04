#include "request.h"
#include "cs_packet.h"

QJsonObject Request::toJson() const
{
    QJsonObject obj;
    switch (type) {
    case REGISTRATION:
        obj.insert("request", QString("registration"));
        break;
    case CHANNEL:
        obj.insert("request", QString("channel"));
        break;
    case CHANNEL_CLOSE:
        obj.insert("request", QString("channel_close"));
        break;
    case VOTE_YES:
        obj.insert("request", QString("vote_yes"));
        break;
    case VOTE_NO:
        obj.insert("request", QString("vote_no"));
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
        return Request(REGISTRATION);
    if (type.toString() == "channel")
        return Request(CHANNEL);
    if (type.toString() == "channel_close")
        return Request(CHANNEL_CLOSE);
    if (type.toString() == "vote_yes")
        return Request(VOTE_YES);
    if (type.toString() == "vote_no")
        return Request(VOTE_NO);

    throw(BadPacket());
}
