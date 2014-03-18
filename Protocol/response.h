#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"
#include <QJsonObject>

struct Response
{
    enum Result {
        SUCCESS,
        ERROR
    };

    Response(Request::Type type = Request::REGISTRATION,
             Result result = ERROR,
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
