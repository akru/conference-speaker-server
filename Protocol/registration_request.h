#ifndef REGISTRATION_REQUEST_H
#define REGISTRATION_REQUEST_H

#include "request.h"
#include "user_information.h"
#include <QJsonObject>

struct RegistrationRequest : public Request
{
    RegistrationRequest(UserInformation user = UserInformation())
        : Request(Request::REGISTRATION),
          user(user)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static RegistrationRequest fromJson(const QJsonObject &json);

    UserInformation user;
};

#endif // REGISTRATION_REQUEST_H
