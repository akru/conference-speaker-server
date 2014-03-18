#ifndef DISCOVER_H
#define DISCOVER_H

#include <QUdpSocket>
#include <server_information.h>

class Discover : public QObject
{
    Q_OBJECT
public:
    explicit Discover(QObject *parent = 0);

signals:
    void discovered(ServerInformation server);

public slots:
    void readPacket();

private:
    QUdpSocket sock;
};

#endif // DISCOVER_H
