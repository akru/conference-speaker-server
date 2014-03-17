#include "server.h"
#include "connection.h"
#include <QTcpSocket>
#include <QDataStream>
#include <cs_packet.h>
#include <server_error.h>

Server::Server(QHostAddress address, quint16 port, QObject *parent)
    : QObject(parent)
{
    connect(&server, SIGNAL(newConnection()), SLOT(newConnection()));
    connect(this, SIGNAL(registrationRequest(Connection*,UserInformation)),
            SLOT(registerUser(Connection*,UserInformation)));

    server.listen(address, port);
}

void Server::newConnection()
{
    // Get first connection
    QTcpSocket *sock = server.nextPendingConnection();
    while (sock)
    {
        qDebug() << "New connection from:"
                 << sock->peerAddress().toString();
        // Create connection instance
        Connection *c = new Connection(sock);
        // Connect to incoming data handler
        connect(c, SIGNAL(readyRead(Connection*)),
                SLOT(connectionReadyRead(Connection*)));
        // Connect to connection destroyer
        connect(c, SIGNAL(disconnected(Connection*)),
                SLOT(connectionClose(Connection*)));

        // Get new connection
        sock = server.nextPendingConnection();
    }
}

void Server::connectionClose(Connection *client)
{
    QString address = client->getAddress();
    qDebug() << "Connection closed:" << address;

    UserInformation user = users[address];
    emit userDisconnected(address, user);

    // Drop user from map
    users.remove(address);
    // Delete connection
    delete client;
}

void Server::connectionReadyRead(Connection *client)
{
    quint8 packetType;
    UserInformation info;

    // Read data and create stream
    QByteArray buffer = client->readAll();
    QDataStream ds(&buffer, QIODevice::ReadOnly);

    // Read packet type
    ds >> packetType;

    switch (packetType)
    {
    case PacketType::REGISTRATION:
        // Parse user info
        ds >> info;
        // Emit signal
        emit registrationRequest(client, info);
        break;
    case PacketType::TRANSMIT:
        emit transmitRequest(client);
        break;
    default:
        // Send failure(unknown request)
        qDebug() << "Unknown request:" << packetType;
        QByteArray res;
        QDataStream ds(&res, QIODevice::WriteOnly);
        QString message("Unknown request");
        ServerError err(message);
        ds << err;
        client->write(res);
    }
}

void Server::registerUser(Connection *client, UserInformation info)
{
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    QString message("Double registration");
    ServerError err(message);

    if (!users.contains(client->getAddress()))
    {
        users.insert(client->getAddress(), info);
        qDebug() << "Success registered user:" << info.name;

        ds << PacketType::RESPONSE_SUCCESS;
        client->write(buffer);

        emit userConnected(client->getAddress(), info);
    }
    else
    {
        qDebug() << "Double registration:" << client->getAddress();

        ds << PacketType::RESPONSE_FAILURE << err;
        client->write(buffer);
    }
}
