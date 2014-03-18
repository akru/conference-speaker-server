#ifndef SERVERINFORMATION_H
#define SERVERINFORMATION_H

#include <QJsonObject>

struct ServerInformation
{
    // Constructor by params
    ServerInformation(QString serverName ="",
                      QString serverAddr = "",
                      quint16 port = 0)
        : name(serverName),
          address(serverAddr),
          port(port)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static ServerInformation fromJson(const QJsonObject &json);

    QString name;
    QString address;
    quint16 port;
};

#endif // SERVERINFORMATION_H
