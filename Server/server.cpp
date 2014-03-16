#include "server.h"
#include "connection.h"
#include "control_packet.h"
#include <QTcpSocket>

Server::Server(quint16 port, QObject *parent) : QObject(parent)
{
    connect(&server, SIGNAL(newConnection()), SLOT(newConnection()));
    connect(this, SIGNAL(registrationRequest(Connection*,UserInformation*)),
            SLOT(registerUser(Connection*,UserInformation*)));

    server.listen(QHostAddress::Any, port);
}

void Server::newConnection()
{
    QTcpSocket *sock = server.nextPendingConnection();
    while (sock)
    {
        qDebug() << "New connection from:" << sock->peerAddress();
        Connection *c = new Connection(sock);
        connect(c, SIGNAL(readyRead(Connection*)), SLOT(connectionReadyRead(Connection*)));
        connections.insert(c->getAddress(), c);
        connect(c, SIGNAL(disconnected(Connection*)), SLOT(connectionClose(Connection*)));

        sock = server.nextPendingConnection();
    }
}

void Server::connectionClose(Connection *client)
{
    qDebug() << "Connection closed:" << client->getAddress();
    connections.remove(client->getAddress());

    UserInformation user = users[client->getAddress()];
    emit userDisconnected(user);

    users.remove(client->getAddress());
    delete client;
}

void Server::connectionReadyRead(Connection *client)
{
    QByteArray buffer = client->readAll();

    QDataStream ds(&buffer, QIODevice::ReadOnly);
    ControlPacket packet(ds);

    UserInformation *info;
    switch (packet.getType())
    {
    case ControlPacket::REGISTRATION:
        info = new UserInformation(ds);
        emit registrationRequest(client, info);
        break;
    case ControlPacket::TRANSMIT:
        emit transmitRequest(client);
        break;
    default:
        // Send failure(unknown request)
        qDebug() << "Unknown request:" << packet.getType();
        QByteArray res = ControlPacket(ControlPacket::RESPONSE_FAILURE).serialize();
        client->write(res);
    }
}

void Server::registerUser(Connection *client, UserInformation *info)
{
    QByteArray res;

    if (!users.contains(client->getAddress()))
    {
        users.insert(client->getAddress(), *info);
        qDebug() << "Success registered user:" << info->getName();
        res = ControlPacket(ControlPacket::RESPONSE_SUCCESS).serialize();
        client->write(res);

        emit userConnected(*info);
    }
    else
    {
        qDebug() << "Double registration:" << client->getAddress();
        res = ControlPacket(ControlPacket::RESPONSE_FAILURE).serialize();
        client->write(res);
    }
    // Free info
    delete info;
}
