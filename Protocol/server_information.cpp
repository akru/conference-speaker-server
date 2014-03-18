#include "server_information.h"
#include "cs_packet.h"

// Json serializer
QJsonObject ServerInformation::toJson() const
{
    QJsonObject obj;
    obj.insert("name", name);
    obj.insert("address", address);
    obj.insert("port", port);
    return obj;
}

// Json deserializer
ServerInformation ServerInformation::fromJson(const QJsonObject &json)
{
    QJsonValue name     = json["name"];
    QJsonValue address  = json["address"];
    QJsonValue port     = json["port"];

    if (name.isUndefined() || address.isUndefined() || port.isUndefined())
        throw(BadPacket());

    return ServerInformation(name.toString(),
                             address.toString(),
                             (quint16)port.toDouble());
}
