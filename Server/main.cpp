#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create main window
    MainWindow w;
    w.show();

    // Exec application
    return a.exec();
}
