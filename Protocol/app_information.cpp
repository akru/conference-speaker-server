#include "app_information.h"
#include "cs_packet.h"

static const QString uriTemplate = "http://%1:%2/app/%3/cs.%4";
static const QString android     = "android";
static const QString ios         = "ios";
static const QString androidExt  = "apk";
static const QString iosExt      = "ipa";

AppInformation::AppInformation(const QString &address, int version)
    : uriAndroid(uriTemplate.arg(address).arg(SERVER_APP_PORT)
                 .arg(android).arg(androidExt)),
      uriIos(uriTemplate.arg(address).arg(SERVER_APP_PORT)
             .arg(ios).arg(iosExt)),
      version(version)
{
}

QJsonObject AppInformation::toJson() const
{
    QJsonObject obj, uri;
    uri.insert(android, uriAndroid);
    uri.insert(ios, uriIos);
    obj.insert("version", version);
    obj.insert("URI", uri);
    return obj;
}


