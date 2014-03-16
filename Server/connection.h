#ifndef CONNECTION_H
#define CONNECTION_H

#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include "user_information.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QTcpSocket *sock, QObject *parent = 0);

    ~Connection();

    QString getAddress() const
    {
        return sock->peerAddress().toString();
    }
    QByteArray readAll()
    {
        return sock->readAll();
    }
    qint64 write(QByteArray &data)
    {
        return sock->write(data);
    }

signals:
    void disconnected(Connection *me);
    void readyRead(Connection *me);

private slots:
    void sockDisconnected()
    {
        emit disconnected(this);
    }
    void sockReadyRead()
    {
        emit readyRead(this);
    }

private:
    QTcpSocket *sock;
};

#endif // CONNECTION_H
