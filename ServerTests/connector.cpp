#include "connector.h"
#include "transmitter.h"
#include <cs_packet.h>
#include <response.h>
#include <channel_response.h>
#include <registration_request.h>
#include <QJsonDocument>

Connector::Connector(QObject *parent)
    : QObject(parent)
{
    connect(&sock, SIGNAL(connected()), SLOT(sockConnected()));
    connect(&sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
    connect(this, SIGNAL(successRegistration()), SLOT(startTransmit()));
}

void Connector::setServer(ServerInformation server)
{
    if (sock.state() == QAbstractSocket::ConnectedState)
        return;
    qDebug() << "connection to" << server.name;
    sock.connectToHost(server.address, server.port);
}

void Connector::sockReadyRead()
{
    QByteArray buffer = sock.readAll();
    Response res;
    try {
        QJsonObject packet = QJsonDocument::fromJson(buffer).object();
        res = Response::fromJson(packet);
    } catch (...) {
        qDebug() << "Bad packet:" << buffer;
        return;
    }

    if (res.type == Request::Registration)
    {
        if (res.result == Response::Success)
        {
            qDebug() << "Success registration";
            emit successRegistration();
        }
        else
            qDebug() << "Registratuion fail:" << res.message;
    }
    if (res.type == Request::ChannelOpen)
    {
        if (res.result == Response::Success)
        {
            try {
                QJsonObject packet =
                        QJsonDocument::fromJson(buffer).object();
                ChannelInformation chan =
                        ChannelResponse::fromJson(packet).channel;
                // Open transmitter
                new Transmitter(chan, this);
                qDebug() << "Success channel:" << chan.toJson();
            } catch (...) {
                qDebug() << "Bad packet:" << buffer;
                return;
            }
        }
        else
            qDebug() << "Channel fail:" << res.message;
    }
}

void Connector::registerUser(UserInformation user)
{
    if (sock.state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "Connector :: not connected";
        return;
    }

    // Send user registration request
    QJsonObject req = RegistrationRequest(user).toJson();
    QByteArray buffer = QJsonDocument(req).toJson();
    sock.write(buffer);
    qDebug() << "Registration request:" << buffer;
}

void Connector::startTransmit()
{
    if (sock.state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "Connector :: not connected";
        return;
    }

    // Send transmit request packet
    Request req(Request::ChannelOpen);
    QByteArray buffer = QJsonDocument(req.toJson()).toJson();
    sock.write(buffer);
    qDebug() << "Channel request:" << buffer;
}
