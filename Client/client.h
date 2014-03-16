#ifndef CLIENT_H
#define CLIENT_H

#include "connector.h"
#include "discover.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);

signals:

public slots:
    void setUserInfo(UserInformation userInfo);
    void serverDiscovered(ServerInformation server);
    void serverConnected();
    void userRegistered();

private:
    UserInformation user;
    Discover discover;
    Connector *connector;
};

#endif // CLIENT_H
