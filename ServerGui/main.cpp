#include "main_window.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator myappTranslator;
    myappTranslator.load(a.applicationDirPath() + "/csgui_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    // Create main window
    MainWindow w;
    w.show();

    // Exec application
    return a.exec();
}
