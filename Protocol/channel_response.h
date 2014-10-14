#ifndef CHANNEL_RESPONSE_H
#define CHANNEL_RESPONSE_H

#include "response.h"
#include "channel_information.h"

struct ChannelResponse : public Response
{
    ChannelResponse(ChannelInformation info = ChannelInformation())
        : Response(Request::Channel, Response::Success),
          channel(info)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static ChannelResponse fromJson(const QJsonObject &json);

    ChannelInformation channel;
};

#endif // CHANNEL_RESPONSE_H
