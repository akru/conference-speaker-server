#include "client.h"

Client::Client(QObject *parent) :
    QObject(parent), user(UserInformation("akru"))
{
    connect(&discover, SIGNAL(discovered(ServerInformation)),
            SLOT(serverDiscovered(ServerInformation)));
}

void Client::setUserInfo(UserInformation userInfo)
{
    user = userInfo;
}

void Client::serverDiscovered(ServerInformation server)
{
    qDebug() << "Server discovered:" << server.getAddress() << ":" << server.getPort();
    disconnect(&discover, SIGNAL(discovered(ServerInformation)), this, 0);
    connector = new Connector();
    connect(connector, SIGNAL(connected()), SLOT(serverConnected()));
    connector->setServer(server);
}

void Client::serverConnected()
{
    qDebug() << "Server connected";
    connect(connector, SIGNAL(succesResponse()), SLOT(userRegistered()));
    connector->registerUser(user);
}

void Client::userRegistered()
{
    disconnect(connector, SIGNAL(succesResponse()), this, 0);
    qDebug() << "User registered";
}
