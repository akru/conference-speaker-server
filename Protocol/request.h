#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonObject>

struct Request
{
    enum Type {
        Registration,
        ChannelOpen,
        ChannelClose,
        Vote
    };
    Request(Type type = Registration)
        : type(type)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static Request fromJson(const QJsonObject &json);

    Type type;
};

#endif // REQUEST_H
