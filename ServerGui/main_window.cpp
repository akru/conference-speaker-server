#include "main_window.h"
#include "ui_main_window.h"
#include "ui_sound_expert_mode.h"
#include "ui_sound_user_mode.h"
#include "speaker_widget.h"
#include "vote_results_widget.h"

#include <gate.h>
#include <user.h>
#include <response.h>
#include <speaker.h>

#include <app_server.h>
#include <qrpage.h>

#include <QFontDatabase>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

/*
const char * statusStoped    = "Server stoped";
const char * statusStarted   = "Server started, waiting for clients...";
const char * statusConnected = "Working, clients connected";

const char * qrPageHeader1   = "Mobile application for your OS:";
const char * qrPageHeader2   = "Glad to hear you!";
*/
const char * statusStoped    = "Сервер выключен.";
const char * statusStarted   = "Сервер запущен, ожидаются подключения...";
const char * statusConnected = "Сервер запущен.";

const char * qrPageHeader1   = "Мобильное приложение для вышей ОС:";
const char * qrPageHeader2   = "Нам важно, чтобы Вы были услышаны!";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    soundExpert(new Ui::SoundExpertMode),
    soundUser(new Ui::SoundUserMode),
    server(0),
    vote(0),
    resultWidget(new VoteResultsWidget)
{
    // Loading fonts
    loadFonts();
    // Setup UI
    ui->setupUi(this);
    soundExpert->setupUi(&wSoundExpert);
    soundUser->setupUi(&wSoundUser);
    // Update network ifaces
    updateAvailAddreses();
    QTimer *t = new QTimer(this);
    t->setInterval(5000);
    connect(t, SIGNAL(timeout()), SLOT(updateAvailAddreses()));
    t->start();
    // Load settings
    settings = new Settings(ui, soundExpert, soundUser, this);
    connect(settings,            SIGNAL(settingsSaved()),
            Speaker::instance(), SLOT(reloadFilterSettings()));
    // Setup UI elements
    setupUi();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete resultWidget;
}

void MainWindow::loadFonts()
{
    qDebug() << "Gothic:"
             << QFontDatabase::addApplicationFont(":/fonts/Gothic.ttf");
    qDebug() << "GothicBold:"
             << QFontDatabase::addApplicationFont(":/fonts/GothicBold.ttf");

    QFont f; f.setFamily("Century Gothic");
    QGuiApplication::setFont(f);

    qDebug() << "Default font:" << QGuiApplication::font().family();
}

void MainWindow::setupUi()
{
    // Sound mode switcher
    connect(ui->rbExpertMode, SIGNAL(clicked()), SLOT(updateSoundMode()));
    connect(ui->rbUserMode,   SIGNAL(clicked()), SLOT(updateSoundMode()));
    updateSoundMode();
    // Scroll boxes alignments
    ui->speakersArea->layout()->setAlignment(Qt::AlignTop);
    // Status label
    setStatus(statusStoped);
    // Separate result widget
    resultWidget->setWindowTitle(tr("Voting results"));
    resultWidget->setStyleSheet(styleSheet());
    resultWidget->setWindowIcon(windowIcon());
    // Two default answer field
    on_plusButton_clicked();
    on_plusButton_clicked();
    // Disable custom answers by default
    on_customRB_toggled(false);
}

void MainWindow::setStatus(const char *status)
{
    ui->statusLabel->setText(status);
}

void MainWindow::updateServerInfo()
{
    if (server)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      tr("Restart server"),
                                      tr("Settings changed, do you want to restart server?"),
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            serverRestart();
    }
}

void MainWindow::userAppend(User *user)
{
    connect(user, SIGNAL(disconnected()), SLOT(userRemove()));
    connect(user, SIGNAL(requestRegistration(UserInformation)),
                  SLOT(requestRegistration(UserInformation)));
    connect(user, SIGNAL(requestChannelOpen()),
                  SLOT(requestChannelOpen()));
    connect(user, SIGNAL(requestChannelClose()),
                  SLOT(requestChannelClose()));
    connect(user, SIGNAL(requestVote(QJsonObject)),
                  SLOT(requestVote(QJsonObject)));
}

void MainWindow::userRemove()
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    // Cleaning user list
    if (userItem.contains(user))
        delete userItem.take(user);
    // Cleaning speaker widget
    if (speakers.contains(user))
        delete speakers.take(user);
}

void MainWindow::requestRegistration(UserInformation info)
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    // Append item into user list
    QListWidgetItem *item = new QListWidgetItem(info.name, ui->userList);
    userItem.insert(user, item);
    // Register user
    user->registration(info);
}

void MainWindow::requestChannelOpen()
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    // Speaker widget exist check
    if (speakers.contains(user))
    {
        qWarning() << "Speaker already exist for user";
    }
    else
    {
        SpeakerWidget *w = new SpeakerWidget(user, this);
        connect(w, SIGNAL(dismiss()), SLOT(channelWidgetRemove()));
        speakers.insert(user, w);
        ui->speakersArea->layout()->addWidget(w);
        w->show();
    }
}
void MainWindow::requestChannelClose()
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    // Send response
    user->channelAction(ChClose);
}

void MainWindow::requestVote(QJsonObject request)
{
    User *user = qobject_cast<User *>(sender());
    // Cast check
    if (!user) return;
    // Check that voting started
    if (!vote)
    {
        Response res(Request::Vote, Response::Error, "Vote not started");
        user->voteResponse(res.toJson());
    }
    else
        user->voteResponse(vote->vote(user->getAddress(), request));
}

void MainWindow::channelWidgetRemove()
{
    SpeakerWidget *w = qobject_cast<SpeakerWidget *>(sender());
    // Cast check
    if (!w) return;
    // Delete when contains
    if (speakers.values().contains(w))
        delete speakers.take(speakers.key(w));
}

void MainWindow::channelWidgetBoost()
{
    SpeakerWidget *w = qobject_cast<SpeakerWidget *>(sender());
    // Cast check
    if (!w) return;
    // Boost widget to top
    ui->speakersArea->layout()->removeWidget(w);
    ((QVBoxLayout *)ui->speakersArea->layout())->insertWidget(0, w);
}

void MainWindow::serverStart()
{
    server = new Gate(this, ui->addressBox->currentText());
    connect(server, SIGNAL(connected(User*)), SLOT(userAppend(User*)));
    // Check gate enabling
    if (!server->isEnabled())
    {
        QMessageBox::critical(this,
                              tr("Server fault"),
                              tr("Unable to start server"));
        serverStop();
        return;
    }
    // Set broadcast information
    broadcaster.setServerInformation(
                ServerInformation(ui->labelName->text(),
                                  ui->addressBox->currentText()));
    broadcaster.start();
    // Connect to appServer
    AppServer::instance()->connectGate(server);
    if (!ui->raPasswordEdit->text().isEmpty())
        AppServer::instance()->
                setAdminPassword(ui->raPasswordEdit->text());
    // Update status
    setStatus(statusStarted);
}

void MainWindow::serverStop()
{
    delete server; server = 0;
    broadcaster.stop();
    broadcaster.unsetVotingInvite();
    setStatus(statusStoped);
}

void MainWindow::on_powerButton_toggled(bool checked)
{
    if (checked)
        serverStart();
    else
        serverStop();
}

void MainWindow::on_recordButton_toggled(bool checked)
{
    if (server)
    {
        if (checked)
            recorder.start();
        else
            recorder.stop();
    }
    else
        ui->recordButton->setChecked(false);
}

void MainWindow::on_startVoteButton_toggled(bool checked)
{
    if (!server) return;

    if (!checked)
    {
        if (vote) { delete vote; vote = 0; }
        broadcaster.unsetVotingInvite();
        ui->startVoteButton->setText(tr("Start"));
    }
    else
    {
        if (ui->simpleRB->isChecked())
            // Simple case
        {
            VotingInvite vi(ui->questionTextEdit->text());
            vote = new Voting(vi, this);
            broadcaster.setVotingInvite(vi);
        }
        else
        {
            QStringList answersText;
            foreach (QLineEdit *l, answers) {
                if (!l->text().isEmpty())
                    answersText.append(l->text());
            }
            // Custom case
            VotingInvite vi(ui->questionTextEdit->text(),
                            VotingInvite::Custom,
                            answersText);
            vote = new Voting(vi, this);
            broadcaster.setVotingInvite(vi);
        }
        connect(vote,         SIGNAL(resultsUpdated(VoteResults)),
                              SLOT(voteUpdateResults(VoteResults)));
        connect(vote,         SIGNAL(resultsUpdated(VoteResults)),
                resultWidget, SLOT(voteUpdateResults(VoteResults)));
        ui->startVoteButton->setText(tr("Stop"));
    }
}

void MainWindow::voteUpdateResults(VoteResults results)
{
    static const char *resultLabelText    = "%1 / %2%";
    // Set question
    ui->resultQuestion->setText(results.invite.question);
    // Clear fields
    while (ui->resultsLayout->itemAt(0) != 0)
    {
        QLayoutItem *i = ui->resultsLayout->takeAt(0);
        delete i->widget();
        delete i;
    }
    // Calc count of members
    int sumAnswers = 0;
    foreach (int a, results.values) {
        sumAnswers += a;
    }
    // Small hack for divByZero exception
    if (!sumAnswers) sumAnswers = 1;
    // Draw fields
    QLabel *answer;
    if (results.invite.mode == VotingInvite::Simple)
    {
        answer = new QLabel(tr(resultLabelText)
                            .arg(results.values[0])
                            .arg(results.values[0] * 100.0 / sumAnswers), this);
        answer->setWordWrap(true);
        ui->resultsLayout->addRow(tr("Yes"), answer);
        answer = new QLabel(tr(resultLabelText)
                            .arg(results.values[1])
                            .arg(results.values[1] * 100.0 / sumAnswers), this);
        answer->setWordWrap(true);
        ui->resultsLayout->addRow(tr("No"), answer);
    }
    else
    {
        for (short i = 0; i < results.values.length() &&
                          i < results.invite.answers.length(); ++i)
        {
            answer = new QLabel(tr(resultLabelText)
                                .arg(results.values[i])
                                .arg(results.values[i] * 100.0 / sumAnswers), this);
            answer->setWordWrap(true);
            ui->resultsLayout->addRow(results.invite.answers[i], answer);
        }
    }
    ui->votingDateEdit->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::on_popupResultsButton_toggled(bool checked)
{
    if (checked)
        resultWidget->show();
    else
        resultWidget->close();
}

void MainWindow::on_plusButton_clicked()
{
    QString name = tr("Answer %1:").arg(ui->customLayout->rowCount() + 1);
    QLineEdit *e = new QLineEdit(this);
    e->setMaxLength(200);
    answers.append(e);
    ui->customLayout->addRow(name, answers.last());
}

void MainWindow::on_customRB_toggled(bool checked)
{
    foreach (QLineEdit *l, answers) {
        l->setEnabled(checked);
    }
    ui->plusButton->setEnabled(checked);
}

void MainWindow::on_drophandsButton_clicked()
{
    foreach (SpeakerWidget *w, speakers) {
        if (w->getState() == SpeakerWidget::Request)
            w->on_dismissButton_clicked();
    }
}

void MainWindow::updateAvailAddreses()
{
    QString cAddr = ui->addressBox->currentText();
    ui->addressBox->clear();
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol
                && !address.isLoopback())
            ui->addressBox->addItem(address.toString());
    }
    ui->addressBox->setCurrentText(cAddr);
}

void MainWindow::on_qrButton_clicked()
{
    ui->qrButton->setCheckable(true);
    ui->qrButton->setChecked(true);
    QRPage p;
    QString addr = "http://" + ui->addressBox->currentText() + ":" + QString::number(SERVER_APP_PORT);
    QString androidAddr = addr + "/app/android/cs.apk";
    QString iosAddr = addr + "/app/ios/cs.ipk";
    p.printPage(addr, androidAddr, iosAddr, tr(qrPageHeader1), tr(qrPageHeader2));
    ui->qrButton->setCheckable(false);
}

void MainWindow::on_storageSelectButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open records directory"));
    recorder.setRecordDirectory(path);
    ui->storageEdit->setText(path);
}

void MainWindow::updateSoundMode()
{
    //
    if (ui->rbUserMode->isChecked())
    { // User mode
        ui->soundProps->layout()->removeWidget(&wSoundExpert);
        ui->soundProps->layout()->addWidget(&wSoundUser);
        wSoundExpert.hide();
        wSoundUser.show();
    }
    else
    { // Expert mode
        ui->soundProps->layout()->removeWidget(&wSoundUser);
        ui->soundProps->layout()->addWidget(&wSoundExpert);
        wSoundUser.hide();
        wSoundExpert.show();
    }
}
