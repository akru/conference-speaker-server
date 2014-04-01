#include "user_information.h"
#include "cs_packet.h"

QJsonObject UserInformation::toJson() const
{
    QJsonObject obj;
    obj.insert("name", name);
    obj.insert("company", company);
    obj.insert("title", title);
    return obj;
}

UserInformation UserInformation::fromJson(const QJsonObject &json)
{
    QJsonValue userName = json["name"];
    QJsonValue userCompany = json["company"];
    QJsonValue userTitle = json["title"];

    if (userName.isUndefined() ||
            userCompany.isUndefined() ||
            userTitle.isUndefined())
        throw(BadPacket());

    return UserInformation(userName.toString(),
                           userCompany.toString(),
                           userTitle.toString());
}
