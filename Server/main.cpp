#include "main_window.h"
#include "broadcaster.h"
#include "server.h"

#include <QNetworkInterface>
#include <QApplication>
#include <cs_packet.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create simple server information
    QHostAddress ip;
    foreach (QHostAddress addr, QNetworkInterface::allAddresses()) {
        if (!addr.isLoopback() &&
                addr.protocol() == QAbstractSocket::IPv4Protocol)
            ip = addr;
        qDebug() << addr;
    }
    ServerInformation serv("Ъ-сервер",
                           ip.toString(), SERVER_CONNECTION_PORT);

    // Create server information broadcaster
    Broadcaster b;
    b.setServerInformation(serv);

    // Create connection server
    Server s(ip);

    // Create main window
    MainWindow w(&s);
    w.show();

    // Exec application
    return a.exec();
}
