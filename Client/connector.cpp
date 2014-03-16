#include "connector.h"

Connector::Connector(QObject *parent)
    : QObject(parent)
{
    connect(&sock, SIGNAL(connected()), SLOT(sockConnected()));
    connect(&sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
}

void Connector::setServer(ServerInformation &server)
{
    sock.connectToHost(server.getAddress(), server.getPort());
}

void Connector::sockReadyRead()
{
    QByteArray buffer = sock.readAll();
    QDataStream ds(&buffer, QIODevice::ReadOnly);
    ControlPacket res(ds);
    if (res.getType() == ControlPacket::RESPONSE_SUCCESS)
        emit succesResponse();
    else
        emit failureResponse();
}

void Connector::registerUser(UserInformation &user)
{
    if (sock.state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "Connector :: not connected";
        return;
    }

    // Send user registration request
    ControlPacket req(ControlPacket::REGISTRATION);
    sock.write(req.serialize() + user.serialize());
}

void Connector::startTransmit()
{
    if (sock.state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "Connector :: not connected";
        return;
    }

    // Send transmit request packet
    sock.write(ControlPacket(ControlPacket::TRANSMIT).serialize());
}
