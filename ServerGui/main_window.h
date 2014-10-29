#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMap>
#include <QMainWindow>
#include <user_information.h>

#include "settings.h"

class Server;
class SpeakerWidget;
class QListWidgetItem;

typedef QMap<QString, QListWidgetItem*> ItemMap;
typedef QMap<QString, SpeakerWidget*>   SpeakerWidgetMap;

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
    void requestAccepted(QString address);
    void requestDiscarded(QString address);

private slots:
    void userAppend(QString address);
    void userRemove(QString address);

    void channelRequest(QString address);
    void channelConnect(QString address);
    void speakerRemove(QString address);

    void serverStart();
    void serverStop();

    inline void serverRestart()
    { serverStop(); serverStart(); }

    void on_addressBox_currentIndexChanged(int index);
    void on_powerButton_toggled(bool checked);
    void on_recordButton_toggled(bool checked);

private:
    Ui::MainWindow  *ui;
    Settings        *settings;
    Server          *server;

    ItemMap          userItem;
    SpeakerWidgetMap speakers;

    void loadFonts();
    void setupUi();
    void setStatus(const char *status);
    void updateServerInfo();
};

#endif // MAIN_WINDOW_H
