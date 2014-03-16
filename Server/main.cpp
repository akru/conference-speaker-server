#include "main_window.h"
#include "broadcaster.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ServerInformation serv("myServer", "127.0.0.1", 12345);
    Broadcaster b;
    b.setServerInformation(serv);

    return a.exec();
}
