#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include "client.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(QUrl("qrc:qml/main.qml"));

    return app.exec();
}
