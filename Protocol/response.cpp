#include "response.h"
#include "cs_packet.h"

QJsonObject Response::toJson() const
{
    QJsonObject obj = Request(type).toJson();
    switch (result) {
    case Success:
        obj.insert("result", QString("success"));
        break;
    case Error:
        obj.insert("result", QString("error"));
        obj.insert("message", message);
        break;
    }
    return obj;
}

Response Response::fromJson(const QJsonObject &json)
{
    Request    req = Request::fromJson(json);
    QJsonValue res = json["result"];
    QJsonValue msg = json["message"];

    if (res.isUndefined())
        throw(BadPacket());

    if (res.toString() == "success")
        return Response(req.type, Success);
    else
    {
        if (msg.isUndefined())
            throw(BadPacket());
        else
            return Response(req.type, Error, msg.toString());
    }
}
