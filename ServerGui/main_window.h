#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMap>
#include <QLineEdit>
#include <QMainWindow>
#include <QListWidgetItem>

#include <user_information.h>
#include <broadcaster.h>
#include <recorder.h>
#include <voting.h>
#include <user.h>

#include "settings.h"

class Gate;
class SpeakerWidget;
class VoteResultsWidget;

typedef QMap<User*, QListWidgetItem*> ItemMap;
typedef QList<QLineEdit*>             AnswerList;
typedef QMap<User*, SpeakerWidget*>   SpeakerWidgetMap;

namespace Ui {
class MainWindow;
class SoundExpertMode;
class SoundUserMode;
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

    void pubDocument(QString route, QByteArray data);

private slots:
    void userAppend(User *user);
    void userRemove();

    void requestRegistration(UserInformation info);
    void requestChannelOpen();
    void requestChannelClose();
    void requestVote(QJsonObject request);

    void channelWidgetRemove();
    void channelWidgetBoost();

    void serverStart();
    void serverStop();
    void serverRestart()
    { serverStop(); serverStart(); }

    void voteUpdateResults(VoteResults results);

    void updateAvailAddreses();
    void updateSoundMode();

    void on_powerButton_toggled(bool checked);
    void on_recordButton_toggled(bool checked);
    void on_startVoteButton_toggled(bool checked);
    void on_popupResultsButton_toggled(bool checked);
    void on_plusButton_clicked();
    void on_customRB_toggled(bool checked);
    void on_drophandsButton_clicked();
    void on_qrButton_clicked();
    void on_storageSelectButton_clicked();
    void on_docButton_clicked();

private:
    Ui::MainWindow      *ui;
    Ui::SoundExpertMode *soundExpert;
    Ui::SoundUserMode   *soundUser;
    QWidget              wSoundExpert, wSoundUser;
    Settings            *settings;
    Gate                *server;
    Voting              *vote;

    ItemMap              userItem;
    SpeakerWidgetMap     speakers;

    VoteResultsWidget   *resultWidget;
    AnswerList           answers;

    Broadcaster          broadcaster;
    Recorder             recorder;

    void loadFonts();
    void setupUi();
    void setStatus(const char *status);
    void updateServerInfo();
};

#endif // MAIN_WINDOW_H
