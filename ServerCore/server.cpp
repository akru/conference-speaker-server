#include "server.h"
#include "broadcaster.h"
#include "connection.h"
#include "receiver.h"
#include "recorder.h"
#include "speaker.h"
#include "voting.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>

#include <cs_packet.h>
#include <request.h>
#include <response.h>
#include <channel_response.h>
#include <registration_request.h>
#include <user_information.h>

#include <QDebug>

Server::Server(const ServerInformation &info, QObject *parent)
    : QObject(parent),
      server(new QTcpServer(this)),
      broadcaster(new Broadcaster(this)),
      voting(0),
      speaker(new Speaker),
      recorder(new Recorder(users))
{
    broadcaster->setServerInformation(info);

    connect(server, SIGNAL(newConnection()), SLOT(connectionNew()));

    connect(this,
            SIGNAL(userRegistrationRequest(QString,UserInformation)),
            SLOT(userRegister(QString,UserInformation)));
    connect(this,
            SIGNAL(channelCloseRequest(QString)),
            SLOT(channelClose(QString)));

    // Connect speaker signals
    connect(this,    SIGNAL(channelConnected(QString)),
            speaker, SLOT(speakerNew(QString)));
    connect(this,    SIGNAL(channelDisconnected(QString)),
            speaker, SLOT(speakerDelete(QString)));
    connect(this,    SIGNAL(channelVolumeChanged(QString,qreal)),
            speaker, SLOT(setVolume(QString,qreal)));
    connect(this,    SIGNAL(channelVolumeChanged(qreal)),
            speaker, SLOT(setVolume(qreal)));
    connect(this,    SIGNAL(channelSettingsUpdated()),
            speaker, SLOT(reloadFilterSettings()));
    connect(speaker, SIGNAL(audioAmpUpdated(QString,ushort)),
            this,    SLOT(channelAmp(QString,ushort)));

    connect(speaker, SIGNAL(sampleReady(QString,QByteArray)),
            recorder,SLOT(record(QString,QByteArray)));
    connect(speaker, SIGNAL(sampleReady(QByteArray)),
            recorder,SLOT(record(QByteArray)));

    // Recorder mgt
    connect(this,     SIGNAL(recordStarted()),
            recorder, SLOT(start()));
    connect(this,     SIGNAL(recordStoped()),
            recorder, SLOT(stop()));

    QHostAddress hostAddress = QHostAddress(info.address);
    // Listening port
    qDebug() << "Listening" << info.address << SERVER_CONNECTION_PORT <<
    server->listen(hostAddress, SERVER_CONNECTION_PORT);
}

Server::~Server()
{
    server->close();
    delete speaker;
    delete recorder;
}

void Server::connectionNew()
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
            connect(c,
                    SIGNAL(readyRead(Connection*)),
                    SLOT(connectionReadyRead(Connection*)));
            // Connect to connection destroyer
            connect(c,
                    SIGNAL(disconnected(Connection*)),
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
        case Request::Registration:
            qDebug() << "New registration request from" << client->getAddress();
            emit userRegistrationRequest(client->getAddress(),
                 RegistrationRequest::fromJson(packet).user);
            break;
        case Request::Channel:
            qDebug() << "New channel request from" << client->getAddress();
            emit channelRequest(client->getAddress());
            break;
        case Request::ChannelClose:
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

void Server::userRegister(QString address, UserInformation info)
{
    Q_ASSERT(clients.contains(address));

    Response res;
    // When users not cointains user record
    if (!users.contains(address))
    {
        users.insert(address, info);
        res = Response(Request::Registration, Response::Success);
        emit userConnected(address);
        qDebug() << "Success registered user:" << info.name;
    }
    else
    {
        qDebug() << "Double registration:" << address;
        res = Response(Request::Registration,
                       Response::Error, "IP already registered");
    }
    // Write response
    clients[address]->write(res.toJson());
}

void Server::userDrop(QString address)
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


void Server::channelDeny(QString address)
{
    Q_ASSERT(clients.contains(address));

    qDebug() << "Channel request denied";
    Response res(Request::Channel, Response::Error, "Access denied");
    QJsonObject result = res.toJson();
    clients[address]->write(result);
}

void Server::channelOpen(QString address)
{
    Q_ASSERT(clients.contains(address));

    QJsonObject result;
    // Unregistered user
    if (!users.contains(address))
    {
        qDebug() << "Unregistered:" << address;
        Response res(Request::Channel, Response::Error, "Please register first");
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
                // Connect with speaker
                connect(r,       SIGNAL(sampleReceived(QString,QByteArray)),
                        speaker, SLOT(incomingData(QString,QByteArray)));
                // Make success response
                ChannelResponse res(r->getChannelInfo());
                result = res.toJson();
                qDebug() << "Success channel open:" << r->getChannelInfo().toJson();
                emit channelConnected(address);
            } catch(...) {
                qDebug() << "Can not open the channel";
                Response res(Request::Channel, Response::Error, "Server fault");
                result = res.toJson();
            }
        }
    }
    clients[address]->write(result);
}

void Server::channelClose(QString address)
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


void Server::channelReloadSettings()
{
    emit channelSettingsUpdated();
}

void Server::voteNew(VotingInvite invite)
{
    // Recreate voting by invite
    delete voting;
    voting = new Voting(invite, this);
    // Connect signals
    connect(this,   SIGNAL(voteRequest(QString,QJsonValue)),
            voting, SLOT(vote(QString,QJsonObject)));
    connect(voting,
            SIGNAL(accepted(QString)),
            SLOT(voteAccept(QString)));
    connect(voting,
            SIGNAL(denied(QString,QString)),
            SLOT(voteDeny(QString,QString)));
    connect(voting,
            SIGNAL(resultsUpdated(VoteResults)),
            SLOT(voteReadyResults(VoteResults)));
    // Set broadcasting message
    broadcaster->setVotingInvite(invite);
}

void Server::voteStop()
{
    delete voting; voting = 0;
    broadcaster->unsetVotingInvite();
}

void Server::voteAccept(QString address)
{
    Q_ASSERT(clients.contains(address));
    Response res(Request::Vote, Response::Success);
    clients[address]->write(res.toJson());
}

void Server::voteDeny(QString address, QString error)
{
    Q_ASSERT(clients.contains(address));
    Response res(Request::Vote, Response::Error, error);
    clients[address]->write(res.toJson());
}

void Server::voteReadyResults(VoteResults results)
{
    emit voteResultsUpdated(results);
}

void Server::channelVolume(QString address, qreal volume)
{
    emit channelVolumeChanged(address, volume);
}

void Server::channelVolume(qreal volume)
{
    emit channelVolumeChanged(volume);
}

void Server::channelAmp(QString address, ushort amp)
{
    emit channelAmpUpdated(address, amp);
}

void Server::recordStart()
{
    emit recordStarted();
}

void Server::recordStop()
{
    emit recordStoped();
}
