#include "registration_request.h"
#include "cs_packet.h"

QJsonObject RegistrationRequest::toJson() const
{
    QJsonObject obj = Request::toJson();
    obj.insert("user", user.toJson());
    return obj;
}

RegistrationRequest RegistrationRequest::fromJson(const QJsonObject &json)
{
    QJsonValue userJson = json["user"];

    if (userJson.isUndefined())
        throw(BadPacket());

    UserInformation userInfo =
            UserInformation::fromJson(userJson.toObject());

    return RegistrationRequest(userInfo);
}
