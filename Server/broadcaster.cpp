#include "broadcaster.h"

Broadcaster::Broadcaster(QObject *parent)
    : QObject(parent),
      server(ServerInformation("test", "127.0.0.1", 15001))
{
    t.setInterval(BROADCASTER_TIME_INTERVAL);
    connect(&t, SIGNAL(timeout()), SLOT(sendInformation()));
    t.start();
}

void Broadcaster::sendInformation()
{
    sock.writeDatagram(server.serialize(),
                       QHostAddress::Broadcast, SERVER_INFORMATION_PORT);
}
