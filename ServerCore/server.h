#ifndef SERVER_H
#define SERVER_H

#include <QMap>
#include <QTcpServer>
#include "user_information.h"
#include "connection.h"
#include "receiver.h"
#include "licensing.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QString &address,
                    QObject *parent = 0);
    ~Server();

signals:
    void userConnected(QString address, UserInformation info);
    void userDisconnected(QString address);
    void channelConnected(QString address, UserInformation user, Receiver *channel);
    void channelDisconnected(QString address);

public slots:
    void denyChannel(QString address);
    void openChannel(QString address);
    void closeChannel(QString address);
    void dropUser(QString address);
    void denyVote(QString address);
    void acceptVote(QString address);
    void reloadFilterSettings();

signals:
    void registrationRequest(QString address, UserInformation info);
    void channelRequest(QString address, UserInformation info);
    void channelCloseRequest(QString address);
    void voteRequest(QString address, bool type);
    void filterSettingsUpdated();

private slots:
    void newConnection();
    void connectionReadyRead(Connection *client);
    void connectionClose(Connection *client);
    void registerUser(QString address, UserInformation info);

private:
    // TCP server instance
    QTcpServer                    *server;
    // User information by address map
    QMap<QString, UserInformation> users;
    // Channel information by address map
    QMap<QString, Receiver *>      channels;
    // Connection by address map
    QMap<QString, Connection *>    clients;
#ifndef QT_DEBUG
    // Licensing policy
    Licensing                      license;
#endif
};

#endif // SERVER_H
