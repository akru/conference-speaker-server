#include <QCoreApplication>
#include <QAudioDeviceInfo>
#include "discover.h"
#include "connector.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QList<QAudioDeviceInfo> devices =
            QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    qDebug() << "Devices:" << devices[0].deviceName();

    Discover d;
    Connector c;
    QObject::connect(&d, SIGNAL(discovered(ServerInformation)),
                     &c, SLOT(setServer(ServerInformation)));
    return a.exec();
}
