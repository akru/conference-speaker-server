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
                SLOT(connectionClosed(Connection*)));

        // Get new connection
        sock = server.nextPendingConnection();
    }
}

void Server::connectionClosed(Connection *client)
{
    QString address = client->getAddress();
    qDebug() << "Connection closed:" << address;

    if (users.contains(address))
    {
        UserInformation user = users[address];
        emit userDisconnected(address, user);
    }
    // Drop user from map
    if (users.contains(address))
        users.remove(address);
    // Close opened channel
    if (channels.contains(address))
    {
        delete channels[address];
        channels.remove(address);
    }
    // Delete client connection
    delete client;
}

void Server::connectionReadyRead(Connection *client)
{
    Request req;
    RegistrationRequest reg_req;
    QJsonDocument packet;

    // Read data and create stream
    QByteArray buffer = client->readAll();

    // Read packet type
    try {
        packet = QJsonDocument::fromJson(buffer);
        req = Request::fromJson(packet.object());
    } catch (BadPacket) {
        qDebug() << "Bad packet: " << buffer;
        return;
    } catch (...) {
        qDebug() << "Unknown error in packet parser";
        return;
    }

    switch (req.type)
    {
    case Request::REGISTRATION:
        qDebug() << "New registration request from" << client->getAddress();
        // Parse registration request
        packet = QJsonDocument::fromJson(buffer);
        reg_req = RegistrationRequest::fromJson(packet.object());
        // Emit signal
        emit registrationRequest(client, reg_req.user);
        break;
    case Request::CHANNEL:
        qDebug() << "New channel request from" << client->getAddress();
        emit channelRequest(client);
        break;
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

    QJsonObject resJson = res.toJson();
    QByteArray buffer = QJsonDocument(resJson).toJson();
    client->write(buffer);
    qDebug() << buffer;
}

void Server::openChannel(Connection *client)
{
    QJsonObject resJson;
    if (!users.contains(client->getAddress()))
    {
        qDebug() << "Unregistered:" << client->getAddress();
        Response res(Request::REGISTRATION,
                       Response::ERROR, "Please register first");
        resJson = res.toJson();
    }
    else
    {
        // Allocate receiver channel
        try {
            Receiver *r = new Receiver();
            // Append to channel map
            channels.insert(client->getAddress(), r);
            // Make success response
            ChannelResponse res(r->getChannel());
            resJson = res.toJson();
            qDebug() << "Success channel open:" << r->getChannel().toJson();
        } catch(...) {
            qDebug() << "Can not open the channel";
            Response res(Request::CHANNEL,
                           Response::ERROR, "Server fault");
            resJson = res.toJson();
        }
    }

    QByteArray buffer = QJsonDocument(resJson).toJson();
    client->write(buffer);
    qDebug() << buffer;
}
