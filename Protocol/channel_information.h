#ifndef CHANNEL_INFORMATION_H
#define CHANNEL_INFORMATION_H

#include <QJsonObject>

struct ChannelInformation
{
    ChannelInformation(QString host = "", quint16 port = 0)
        : host(host), port(port)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static ChannelInformation fromJson(const QJsonObject &json);

    QString host;
    quint16 port;
};

#endif // CHANNEL_INFORMATION_H
