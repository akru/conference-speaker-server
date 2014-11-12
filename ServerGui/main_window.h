#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMap>
#include <QMainWindow>
#include <user_information.h>
#include <voting.h>

#include "settings.h"

class Server;
class QLineEdit;
class SpeakerWidget;
class QListWidgetItem;
class VoteResultsWidget;

typedef QMap<QString, QListWidgetItem*> ItemMap;
typedef QList<QLineEdit*>               AnswerList;
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

    void voteStop();
    void voteNew(VotingInvite);

private slots:
    void userAppend(QString address);
    void userRemove(QString address);

    void channelRequest(QString address);
    void channelConnect(QString address);
    void speakerRemove(QString address);

    void serverStart();
    void serverStop();
    void serverRestart()
    { serverStop(); serverStart(); }

    void voteUpdateResults(VoteResults results);

    void updateAvailAddreses();

    void on_powerButton_toggled(bool checked);
    void on_recordButton_toggled(bool checked);
    void on_startVoteButton_toggled(bool checked);
    void on_popupResultsButton_toggled(bool checked);
    void on_plusButton_clicked();
    void on_customRB_toggled(bool checked);
    void on_drophandsButton_clicked();
    void on_qrButton_clicked();

    void on_storageSelectButton_clicked();

private:
    Ui::MainWindow    *ui;
    Settings          *settings;
    Server            *server;

    ItemMap            userItem;
    SpeakerWidgetMap   speakers;

    VoteResultsWidget *resultWidget;
    AnswerList         answers;

    void loadFonts();
    void setupUi();
    void setStatus(const char *status);
    void updateServerInfo();
};

#endif // MAIN_WINDOW_H
