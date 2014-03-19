#include "main_window.h"
#include "broadcaster.h"
#include "server.h"

#include <QApplication>
#include <cs_packet.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create simple server information
    ServerInformation serv("Ъ-сервер",
                           "172.17.0.12", SERVER_CONNECTION_PORT);

    // Create server information broadcaster
    Broadcaster b;
    b.setServerInformation(serv);

    // Create connection server
    Server s(QHostAddress::AnyIPv4, SERVER_CONNECTION_PORT);

    // Create main window
    MainWindow w(&s);
    w.show();

    // Exec application
    return a.exec();
}
