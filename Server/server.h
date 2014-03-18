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
    explicit Server(QHostAddress address,
                    quint16 port,
                    QObject *parent = 0);

signals:
    void userConnected(QString, UserInformation);
    void userDisconnected(QString, UserInformation);

public slots:


signals:
    void registrationRequest(Connection *, UserInformation);
    void channelRequest(Connection *);

private slots:
    void newConnection();
    void connectionReadyRead(Connection *client);
    void connectionClose(Connection *client);
    void registerUser(Connection *client, UserInformation info);

private:
    // TCP server instance
    QTcpServer server;
    // User information by address map
    QMap<QString, UserInformation> users;
};

#endif // SERVER_H
