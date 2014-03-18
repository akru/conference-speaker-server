#ifndef CHANNEL_INFORMATION_H
#define CHANNEL_INFORMATION_H

#include <QJsonObject>

struct ChannelInformation
{
    ChannelInformation(quint16 port = 0)
        : port(port)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static ChannelInformation fromJson(const QJsonObject &json);

    quint16 port;
};

#endif // CHANNEL_INFORMATION_H
