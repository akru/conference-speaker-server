#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <QTimer>
#include <QUdpSocket>

#include "server_information.h"

const int BROADCASTER_TIME_INTERVAL = 1000;

class Broadcaster : public QObject
{
    Q_OBJECT
public:
    explicit Broadcaster(QObject *parent = 0);

signals:

public slots:
    void setServerInformation(ServerInformation &info)
    {
        server = info;
    }

    void sendInformation();

private:
    ServerInformation server;
    QUdpSocket sock;
    QTimer t;
};

#endif // BROADCASTER_H
