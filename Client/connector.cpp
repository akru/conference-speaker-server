#include "connector.h"
#include <cs_packet.h>
#include <server_error.h>

Connector::Connector(QObject *parent)
    : QObject(parent)
{
    connect(&sock, SIGNAL(connected()), SLOT(sockConnected()));
    connect(&sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
}

void Connector::setServer(ServerInformation &server)
{
    sock.connectToHost(server.address, server.port);
}

void Connector::sockReadyRead()
{
    QByteArray buffer = sock.readAll();
    QDataStream ds(&buffer, QIODevice::ReadOnly);
    ServerError err;
    quint8 packetType;
    ds >> packetType;
    if (packetType == PacketType::RESPONSE_SUCCESS)
        emit succesResponse();
    else
    {
        ds >> err;
        emit failureResponse(err.message);
    }
}

void Connector::registerUser(UserInformation &user)
{
    if (sock.state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "Connector :: not connected";
        return;
    }

    // Send user registration request
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << PacketType::REGISTRATION << user;
    sock.write(buffer);
}

void Connector::startTransmit()
{
    if (sock.state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "Connector :: not connected";
        return;
    }

    // Send transmit request packet
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << PacketType::TRANSMIT;
    sock.write(buffer);
}
