#include <QCoreApplication>
#include <QAudioDeviceInfo>
#include "discover.h"
#include "connector.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Discover d;
    Connector c;
    QObject::connect(&d, SIGNAL(discovered(ServerInformation)),
                     &c, SLOT(setServer(ServerInformation)));
    return a.exec();
}
