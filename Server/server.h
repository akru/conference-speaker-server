#ifndef SERVER_H
#define SERVER_H

#include <QMap>
#include <QTcpServer>
#include "user_information.h"
#include "connection.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(quint16 port, QObject *parent = 0);

signals:
    void registrationRequest(Connection *client, UserInformation *info);
    void transmitRequest(Connection *client);

    void userConnected(UserInformation info);
    void userDisconnected(UserInformation info);

public slots:
    void newConnection();
    void connectionReadyRead(Connection *client);
    void connectionClose(Connection *client);
    void registerUser(Connection *client, UserInformation *info);

private:
    QTcpServer server;
    QMap<QString, UserInformation> users;
    QMap<QString, Connection*> connections;
};

#endif // SERVER_H
