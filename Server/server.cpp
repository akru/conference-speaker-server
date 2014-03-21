#include "server.h"
#include "connection.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <cs_packet.h>
#include <request.h>
#include <response.h>
#include <channel_response.h>
#include <registration_request.h>
#include <user_information.h>

Server::Server(QHostAddress address, QObject *parent)
    : QObject(parent)
{
    connect(&server, SIGNAL(newConnection()), SLOT(newConnection()));
    connect(this, SIGNAL(registrationRequest(Connection*,UserInformation)),
                  SLOT(registerUser(Connection*,UserInformation)));

    // Listening port
    server.listen(address, SERVER_CONNECTION_PORT);
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

    // Close opened connections
    if (users.contains(address))
    {
        emit userDisconnected(address);

        // Drop user connection and info from map
        delete client;
        users.remove(address);
    }
    // Close opened channel
    if (channels.contains(address))
    {
        // Drop channel from map
        delete channels[address];
        channels.remove(address);
    }
    qDebug() << "Connection closed:" << address;
}

void Server::connectionReadyRead(Connection *client)
{
    // Read data and create stream
    QByteArray buffer = client->readAll();

    // Read packet type
    try {
        QJsonDocument packet = QJsonDocument::fromJson(buffer);
        Request          req = Request::fromJson(packet.object());
        switch (req.type)
        {
        case Request::REGISTRATION:
            qDebug() << "New registration request from" << client->getAddress();
            emit registrationRequest(client,
                 RegistrationRequest::fromJson(packet.object()).user);
            break;
        case Request::CHANNEL:
            qDebug() << "New channel request from" << client->getAddress();
            emit channelRequest(client, users[client->getAddress()]);
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
    Response res;
    if (!users.contains(client->getAddress()))
    {
        users.insert(client->getAddress(), info);
        qDebug() << "Success registered user:" << info.name;
        res = Response(Request::REGISTRATION, Response::SUCCESS);;
        emit userConnected(client->getAddress(), info);
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
    QJsonObject result;
    // Unregistered user
    if (!users.contains(client->getAddress()))
    {
        qDebug() << "Unregistered:" << client->getAddress();
        Response res(Request::REGISTRATION,
                       Response::ERROR, "Please register first");
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
                Receiver *r = new Receiver(server.serverAddress());
                // Append to channel map
                channels.insert(client->getAddress(), r);
                // Make success response
                ChannelResponse res(r->getChannelInfo());
                result = res.toJson();
                qDebug() << "Success channel open:"
                         << r->getChannelInfo().toJson();
                // Emit new channel notification
                emit channelConnected(users[client->getAddress()], r);
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
    delete channels[address];
    channels.remove(address);
}
