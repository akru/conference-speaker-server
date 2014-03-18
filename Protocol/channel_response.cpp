#include "channel_response.h"
#include "cs_packet.h"

QJsonObject ChannelResponse::toJson() const
{
    QJsonObject obj = Response::toJson();
    obj.insert("channel", channel.toJson());
    return obj;
}

ChannelResponse ChannelResponse::fromJson(const QJsonObject &json)
{
    QJsonValue channelJson = json["channel"];

    if (channelJson.isUndefined())
        throw(BadPacket());

    ChannelInformation channelInfo =
            ChannelInformation::fromJson(channelJson.toObject());

    return ChannelResponse(channelInfo);
}
