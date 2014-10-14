#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"
#include <QJsonObject>

struct Response
{
    enum Result {
        Success,
        Error
    };

    Response(Request::Type type = Request::Registration,
             Result result = Error,
             QString msg = "")
        : type(type),
          result(result),
          message(msg)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static Response fromJson(const QJsonObject &json);

    Request::Type type;
    Result result;
    QString message;
};

#endif // RESPONSE_H
