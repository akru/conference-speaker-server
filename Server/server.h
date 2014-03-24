#ifndef SERVER_H
#define SERVER_H

#include <QMap>
#include <QTcpServer>
#include "user_information.h"
#include "connection.h"
#include "receiver.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QHostAddress address,
                    QObject *parent = 0);

signals:
    void userConnected(QString address, UserInformation info);
    void userDisconnected(QString address);
    void channelConnected(UserInformation user, Receiver *channel);

public slots:
    void denyChannel(Connection *client);
    void openChannel(Connection *client);
    void closeChannel(QString address);

signals:
    void registrationRequest(Connection *client, UserInformation info);
    void channelRequest(Connection *client, UserInformation info);

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
    // Channel information by address map
    QMap<QString, Receiver *> channels;
};

#endif // SERVER_H
