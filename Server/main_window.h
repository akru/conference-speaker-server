#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMap>
#include <QMainWindow>
#include <user_information.h>

#include "settings_dialog.h"
#include "channel_widget.h"
#include "about_dialog.h"
#include "receiver.h"
#include "broadcaster.h"
#include "server.h"

class Server;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void channelRequestAccepted(Connection *);
    void channelRequestDiscarded(Connection *);

public slots:

private slots:
    void appendUser(QString address, UserInformation info);
    void dropUser(QString address);
    void appendChannel(QString address, UserInformation info, Receiver *channel);
    void dropChannel(QString address);
    void channelRequest(Connection *client, UserInformation info);
    void on_actionAbout_triggered();
    void on_actionSettings_triggered();

    void updateServerInfo(ServerInformation info);

private:
    Ui::MainWindow *ui;
    SettingsDialog settings;
    AboutDialog about;

    Server *server;
    Broadcaster broadcaster;
    QMap<QString, ChannelWidget*> channels;
};

#endif // MAIN_WINDOW_H
