#ifndef CONNECTION_H
#define CONNECTION_H

#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include "user_information.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QTcpSocket *sock, QObject *parent = 0);

    QString getAddress() const
    {
        return sock->peerAddress().toString();
    }
    QByteArray readAll()
    {
        return sock->readAll();
    }
    qint64 write(QJsonObject json)
    {
        QByteArray data = QJsonDocument(json).toJson();
        return sock->write(data);
    }

signals:
    void disconnected(Connection *);
    void readyRead(Connection *);

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
