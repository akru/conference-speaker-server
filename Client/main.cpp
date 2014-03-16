#include "qtquick1applicationviewer.h"
#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QtQuick1ApplicationViewer viewer;
    viewer.addImportPath(QLatin1String("modules"));
    viewer.setOrientation(QtQuick1ApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/Client/main.qml"));
    viewer.showExpanded();

    Client client;

    return app.exec();
}
