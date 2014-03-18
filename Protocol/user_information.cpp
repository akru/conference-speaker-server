#include "user_information.h"
#include "cs_packet.h"

QJsonObject UserInformation::toJson() const
{
    QJsonObject obj;
    obj.insert("name", name);
    return obj;
}

UserInformation UserInformation::fromJson(const QJsonObject &json)
{
    QJsonValue userName = json["name"];

    if (userName.isUndefined())
        throw(BadPacket());

    return UserInformation(userName.toString());
}
