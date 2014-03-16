#include "connection.h"

Connection::Connection(QTcpSocket *sock, QObject *parent)
    : QObject(parent),
      sock(sock)
{
    connect(sock, SIGNAL(disconnected()), SLOT(sockDisconnected()));
    connect(sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
}

Connection::~Connection()
{
    delete sock;
}
