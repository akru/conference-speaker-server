#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMap>
#include <QMainWindow>
#include <user_information.h>

#ifdef QT_DEBUG
#include "debug_dialog.h"
#endif
#include "settings_dialog.h"
#include "channel_widget.h"
#include "request_widget.h"
#include "client_widget.h"
#include "about_dialog.h"
#include "debug_dialog.h"
#include "vote_widget.h"
#include "broadcaster.h"
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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void channelRequestAccepted(QString address);
    void channelRequestDiscarded(QString address);

public slots:

private slots:
    void appendClient(QString address, UserInformation info);
    void dropClient(QString address);
    void appendChannel(QString address, UserInformation info, Receiver *channel);
    void dropChannel(QString address);
    void channelRequest(QString address, UserInformation info);
    void dropRequest(QString address);
    void on_actionAbout_triggered();
    void on_actionSettings_triggered();
    void on_actionSound_processing_triggered();

    void updateServerInfo(ServerInformation info);

private:
    Ui::MainWindow *ui;
    SettingsDialog settings;
    DebugDialog filter_setup;
    AboutDialog about;
    VoteWidget voting;

    Server *server;
    Broadcaster broadcaster;

    QMap<QString, ClientWidget*>  clients;
    QMap<QString, RequestWidget*> requests;
    QMap<QString, ChannelWidget*> channels;
};

#endif // MAIN_WINDOW_H
