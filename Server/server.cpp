#include "server.h"
#include "connection.h"
#include <QTcpSocket>
#include <QJsonDocument>
#include <cs_packet.h>
#include <request.h>
#include <response.h>
#include <channel_response.h>
#include <registration_request.h>
#include <user_information.h>

Server::Server(QString &address, QObject *parent)
    : QObject(parent), server(new QTcpServer)
{
    connect(server, SIGNAL(newConnection()), SLOT(newConnection()));
    connect(this, SIGNAL(registrationRequest(Connection*,UserInformation)),
                  SLOT(registerUser(Connection*,UserInformation)));
    connect(this, SIGNAL(channelCloseRequest(QString)), SLOT(closeChannel(QString)));

    QHostAddress hostAddress = QHostAddress(address);
    // Listening port
    qDebug() << "Listening" << address << SERVER_CONNECTION_PORT <<
    server->listen(hostAddress, SERVER_CONNECTION_PORT);
}

Server::~Server()
{
    server->close();
    delete server;
}

void Server::newConnection()
{
    // Get first connection
    QTcpSocket *sock = server->nextPendingConnection();
    while (sock)
    {
        qDebug() << "New connection from"
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
        sock = server->nextPendingConnection();
    }
}

void Server::connectionClose(Connection *client)
{
    Q_ASSERT(client);

    QString address = client->getAddress();

    // Close opened connections
    if (users.contains(address))
    {
        emit userDisconnected(address);

        // Drop user connection and info from map
        users.remove(address);
    }
    // Close opened channel
    if (channels.contains(address))
    {
        emit channelDisconnected(address);
        // Drop channel voice receiver
        delete channels[address];
        // Drop channel from map
        channels.remove(address);
    }

    // Drop connection
    delete client;
    qDebug() << "Connection closed:" << address;
}

void Server::connectionReadyRead(Connection *client)
{
    Q_ASSERT(client);

    // Read data and create stream
    QByteArray buffer = client->readAll();

    // Read packet type
    try {
        QJsonObject   packet = QJsonDocument::fromJson(buffer).object();
        Request          req = Request::fromJson(packet);
        switch (req.type)
        {
        case Request::REGISTRATION:
            qDebug() << "New registration request from" << client->getAddress();
            emit registrationRequest(client,
                 RegistrationRequest::fromJson(packet).user);
            break;
        case Request::CHANNEL:
            qDebug() << "New channel request from" << client->getAddress();
            emit channelRequest(client, users[client->getAddress()]);
            break;
        case Request::CHANNEL_CLOSE:
            qDebug() << "New channel close request from" << client->getAddress();
            emit channelCloseRequest(client->getAddress());
            break;
        }
    } catch (BadPacket) {
        qDebug() << "Bad packet: " << buffer;
        return;
    } catch (...) {
        qDebug() << "Unknown error in packet parser";
        return;
    }
}

void Server::registerUser(Connection *client, UserInformation info)
{
    Q_ASSERT(client);

    Response res;
    // When users not cointains user record
    if (!users.contains(client->getAddress()))
    {
        users.insert(client->getAddress(), info);
        res = Response(Request::REGISTRATION, Response::SUCCESS);
        emit userConnected(client->getAddress(), info);
        qDebug() << "Success registered user:" << info.name;
    }
    else
    {
        qDebug() << "Double registration:" << client->getAddress();
        res = Response(Request::REGISTRATION,
                       Response::ERROR, "IP already registered");
    }
    client->write(res.toJson());
}

void Server::denyChannel(Connection *client)
{
    qDebug() << "Channel request denied";
    Response res(Request::CHANNEL, Response::ERROR, "Access denied");
    QJsonObject result = res.toJson();
    client->write(result);
}

void Server::openChannel(Connection *client)
{
    Q_ASSERT(client);

    QJsonObject result;
    // Unregistered user
    if (!users.contains(client->getAddress()))
    {
        qDebug() << "Unregistered:" << client->getAddress();
        Response res(Request::CHANNEL, Response::ERROR, "Please register first");
        result = res.toJson();
    }
    else
    {
        // Existed channel
        if (channels.contains(client->getAddress()))
        {
            // Return existed channel info
            Receiver *r = channels[client->getAddress()];
            ChannelResponse res(r->getChannelInfo());
            result = res.toJson();
        }
        // New channel
        else
        {
            // Allocate voice receiver
            try {
                Receiver *r = new Receiver(server->serverAddress());
                // Append to channel map
                channels.insert(client->getAddress(), r);
                // Make success response
                ChannelResponse res(r->getChannelInfo());
                result = res.toJson();
                qDebug() << "Success channel open:" << r->getChannelInfo().toJson();
                emit channelConnected(client->getAddress(), users[client->getAddress()], r);
            } catch(...) {
                qDebug() << "Can not open the channel";
                Response res(Request::CHANNEL, Response::ERROR, "Server fault");
                result = res.toJson();
            }
        }
    }
    client->write(result);
}

void Server::closeChannel(QString address)
{
    Q_ASSERT(channels.contains(address));
    // Delete voice receiver instance
    delete channels[address];
    // Drop channel from map
    channels.remove(address);
    // Emit disconnected signal
    emit channelDisconnected(address);
}
