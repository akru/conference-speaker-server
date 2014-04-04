#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonObject>

struct Request
{
    enum Type {
        REGISTRATION,
        CHANNEL,
        CHANNEL_CLOSE,
        VOTE_YES,
        VOTE_NO
    };
    Request(Type type = REGISTRATION)
        : type(type)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static Request fromJson(const QJsonObject &json);

    Type type;
};

#endif // REQUEST_H
