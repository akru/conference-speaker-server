#include "gate.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

Gate::Gate(QObject *parent,
           const QString &listeningAddress,
           quint16 listeningPort)
    : QObject(parent),
      server(new QTcpServer(this))
{
    // Incoming connections handler
    connect(server, SIGNAL(newConnection()), SLOT(newConnection()));
    // Listening the port
    listenEnabled = server->listen(QHostAddress(listeningAddress),
                                   listeningPort);
    qDebug() << "GATE :: listen"
             << listeningAddress << listeningPort << listenEnabled;
}

void Gate::newConnection()
{
    // Get first connection
    QTcpSocket *sock = server->nextPendingConnection();
    // ConLimit check
    if (users.count() > USER_LIMIT)
    {
        qWarning() << "GATE :: User limit has been reached!";
        qWarning() << "GATE :: limit is " << USER_LIMIT
                   << ", count of users is " << users.count();
        return;
    }
    // New connection handler
    while (sock)
    {
        qDebug() << "GATE :: New connection from"
                 << sock->peerAddress().toString();
        // Create User instance
        User *u = new User(sock, this);
        // ID collision check
        if (users.contains(u->getAddress()))
        {
            qWarning() << "GATE :: User address collision:"
                       << u->getAddress();
            // Drop double user
            delete u;
        }
        else
        {
            // Store connected user
            users.append(u->getAddress());
            connect(u, SIGNAL(disconnected()), SLOT(userDisconnected()));
            // Emit connected signal
            emit connected(u);
        }
        // Get new connection
        sock = server->nextPendingConnection();
    }
}

void Gate::userDisconnected()
{
    User *u = qobject_cast<User *>(sender());
    if (u) users.removeOne(u->getAddress());
    // TODO: after deleting object may be used in other code =\
    //delete u;
}
