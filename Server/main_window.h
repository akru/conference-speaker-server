#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <user_information.h>

#include "about_dialog.h"

class Server;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Server *server, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void appendUser(QString address, UserInformation info);
    void dropUser(QString address, UserInformation info);

private slots:
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    AboutDialog about;

    Server *server;
};

#endif // MAIN_WINDOW_H
