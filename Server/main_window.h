#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <user_information.h>

#include "about_dialog.h"
#include "receiver.h"
#include "server.h"

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

signals:
    void channelRequestAccepted(Connection *);
    void channelRequestDiscarded(Connection *);

public slots:

private slots:
    void appendUser(QString address, UserInformation info);
    void dropUser(QString address);
    void appendChannel(UserInformation info, Receiver *channel);
    void channelRequest(Connection *client, UserInformation info);
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    AboutDialog about;

    Server *server;
};

#endif // MAIN_WINDOW_H
