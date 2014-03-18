#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonObject>

struct Request
{
    enum Type {
        REGISTRATION,
        CHANNEL
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
