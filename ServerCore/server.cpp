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
#include <QDebug>

Server::Server(QString &address, QObject *parent)
    : QObject(parent), server(new QTcpServer)
{
    connect(server, SIGNAL(newConnection()), SLOT(newConnection()));
    connect(this, SIGNAL(registrationRequest(QString,UserInformation)),
                  SLOT(registerUser(QString,UserInformation)));
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
#ifndef QT_DEBUG
    // Licensing policy for max connections
    if (license.getMaxConnections() &&
            clients.size() >= license.getMaxConnections())
    {
        sock->close();
        return;
    }
#endif
    while (sock)
    {
        if (!clients.contains(sock->peerAddress().toString()))
        {
            qDebug() << "New connection from"
                     << sock->peerAddress().toString();
            // Create connection instance
            Connection *c = new Connection(sock, this);
            // Connect to incoming data handler
            connect(c, SIGNAL(readyRead(Connection*)),
                    SLOT(connectionReadyRead(Connection*)));
            // Connect to connection destroyer
            connect(c, SIGNAL(disconnected(Connection*)),
                    SLOT(connectionClose(Connection*)));
            // Store connection pointer
            clients[c->getAddress()] = c;
        }

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
    clients.remove(client->getAddress());
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
            emit registrationRequest(client->getAddress(),
                 RegistrationRequest::fromJson(packet).user);
            break;
        case Request::CHANNEL:
            qDebug() << "New channel request from" << client->getAddress();
            emit channelRequest(client->getAddress(), users[client->getAddress()]);
            break;
        case Request::CHANNEL_CLOSE:
            qDebug() << "New channel close request from" << client->getAddress();
            emit channelCloseRequest(client->getAddress());
            break;
        case Request::VOTE_YES:
            qDebug() << "New vote yes request from" << client->getAddress();
            if (users.contains(client->getAddress()))
                emit voteRequest(client->getAddress(), true);
            else
                denyVote(client->getAddress());
            break;
        case Request::VOTE_NO:
            qDebug() << "New vote no request from" << client->getAddress();
            emit voteRequest(client->getAddress(), false);
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

void Server::registerUser(QString address, UserInformation info)
{
    Q_ASSERT(clients.contains(address));

    Response res;
    // When users not cointains user record
    if (!users.contains(address))
    {
        users.insert(address, info);
        res = Response(Request::REGISTRATION, Response::SUCCESS);
        emit userConnected(address, info);
        qDebug() << "Success registered user:" << info.name;
    }
    else
    {
        qDebug() << "Double registration:" << address;
        res = Response(Request::REGISTRATION,
                       Response::ERROR, "IP already registered");
    }
    clients[address]->write(res.toJson());
}

void Server::denyChannel(QString address)
{
    Q_ASSERT(clients.contains(address));

    qDebug() << "Channel request denied";
    Response res(Request::CHANNEL, Response::ERROR, "Access denied");
    QJsonObject result = res.toJson();
    clients[address]->write(result);
}

void Server::openChannel(QString address)
{
    Q_ASSERT(clients.contains(address));

    QJsonObject result;
    // Unregistered user
    if (!users.contains(address))
    {
        qDebug() << "Unregistered:" << address;
        Response res(Request::CHANNEL, Response::ERROR, "Please register first");
        result = res.toJson();
    }
    else
    {
        // Existed channel
        if (channels.contains(address))
        {
            // Return existed channel info
            Receiver *r = channels[address];
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
                channels.insert(address, r);
                // Make success response
                ChannelResponse res(r->getChannelInfo());
                result = res.toJson();
                qDebug() << "Success channel open:" << r->getChannelInfo().toJson();
                emit channelConnected(address, users[address], r);
            } catch(...) {
                qDebug() << "Can not open the channel";
                Response res(Request::CHANNEL, Response::ERROR, "Server fault");
                result = res.toJson();
            }
        }
    }
    clients[address]->write(result);
}

void Server::dropUser(QString address)
{
    qDebug() << "Drop user at address" << address;
    if (channels.contains(address))
    {
        delete channels[address];
        channels.remove(address);
    }
    if (users.contains(address))
        users.remove(address);
    if (clients.contains(address))
    {
        delete clients[address];
        clients.remove(address);
    }
    emit userDisconnected(address);
}

void Server::closeChannel(QString address)
{
    qDebug() << "Close channel at address" << address;
    if(channels.contains(address))
    {
        // Delete voice receiver instance
        delete channels[address];
        // Drop channel from map
        channels.remove(address);
        // Emit disconnected signal
        emit channelDisconnected(address);
    }
}

void Server::acceptVote(QString address)
{
    Q_ASSERT(clients.contains(address));

    qDebug() << "Channel request accepted";
    Response res(Request::VOTE_YES, Response::SUCCESS); // TODO: Vote request
    QJsonObject result = res.toJson();
    clients[address]->write(result);
}

void Server::denyVote(QString address)
{
    Q_ASSERT(clients.contains(address));

    qDebug() << "Vote request denied";
    Response res(Request::VOTE_YES, Response::ERROR, "Access denied");
    QJsonObject result = res.toJson();
    clients[address]->write(result);
}
