#include "main_window.h"
#include "ui_main_window.h"
#include "speaker_widget.h"

#include <server.h>

#include <QFontDatabase>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMessageBox>
#include <QSettings>

const char * statusStoped    = "Server stoped";
const char * statusStarted   = "Server started, waiting for clients...";
const char * statusConnected = "Working, clients connected";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(0),
    resultWidget(new QWidget)
{
    // Loading fonts
    loadFonts();
    // Setup UI
    ui->setupUi(this);
    // Load settings
    settings = new Settings(ui, this);
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
    // Conference name label
    ui->labelName->setText(settings->info.name);
    connect(ui->powerButton, SIGNAL(clicked()),
            settings,        SLOT(save()));
    // IP address box
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol
                && !address.isLoopback())
        ui->addressBox->addItem(address.toString());
    }
    int addrId = ui->addressBox->findText(settings->info.address);
    ui->addressBox->setCurrentIndex(addrId);
    // Scroll boxes alignments
    ui->speakersArea->layout()->setAlignment(Qt::AlignTop);
    // Status label
    setStatus(statusStoped);
    // Separate result widget
    resultWidget->setLayout(new QHBoxLayout(resultWidget));
    resultWidget->setWindowTitle(tr("Voting results"));
    resultWidget->setWindowFlags(Qt::WindowTitleHint);
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
    ui->statusLabel->setText(tr(status));
}

void MainWindow::updateServerInfo()
{
    settings->info.name = ui->labelName->text();
    settings->info.address = ui->addressBox->currentText();
    settings->save();

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

void MainWindow::userAppend(QString address)
{
    Q_ASSERT(!userItem.contains(address) && server->getUsers().contains(address));
    // Client label in client list
    QString label = server->getUsers()[address].name;
    QListWidgetItem *item = new QListWidgetItem(label);
    ui->userList->addItem(item);
    userItem.insert(address, item);
    // Update status
    setStatus(statusConnected);
}

void MainWindow::userRemove(QString address)
{
    Q_ASSERT(userItem.contains(address));
    // Drop client from list
    delete userItem.take(address);
}

void MainWindow::channelConnect(QString address)
{
    Q_ASSERT(speakers.contains(address));
    SpeakerWidget *w = speakers[address];

    connect(w,      SIGNAL(volumeChanged(QString,qreal)),
            server, SLOT(channelVolume(QString,qreal)));
    connect(server, SIGNAL(channelAmpUpdated(QString,ushort)),
            w,      SLOT(setAmplitude(QString,ushort)));

    connect(w,      SIGNAL(closeClicked(QString)),
            server, SLOT(channelClose(QString)));
    connect(w,
            SIGNAL(closeClicked(QString)),
            SLOT(speakerRemove(QString)));

    // Boost widget to top
    ui->speakersArea->layout()->removeWidget(w);
    ((QVBoxLayout *)ui->speakersArea->layout())->insertWidget(0, w);

    w->setState(SpeakerWidget::Stream);
}

void MainWindow::speakerRemove(QString address)
{
    Q_ASSERT(speakers.contains(address));
    delete speakers.take(address);
}

void MainWindow::channelRequest(QString address)
{
    Q_ASSERT(!speakers.contains(address) && server->getUsers().contains(address));

    SpeakerWidget *w = new SpeakerWidget(address,
                                         server->getUsers()[address],
                                         this);
    speakers.insert(address, w);

    connect(w,      SIGNAL(requestAccepted(QString)),
            server, SLOT(channelOpen(QString)));
    connect(w,      SIGNAL(requestDiscarded(QString)),
            server, SLOT(channelDeny(QString)));
    connect(w,
            SIGNAL(requestDiscarded(QString)),
            SLOT(speakerRemove(QString)));

    ui->speakersArea->layout()->addWidget(w);
    w->show();
}

void MainWindow::serverStart()
{
    server = new Server(settings->info);
    // User MGT
    connect(server,
            SIGNAL(userConnected(QString)),
            SLOT(userAppend(QString)));
    connect(server,
            SIGNAL(userDisconnected(QString)),
            SLOT(userRemove(QString)));
    // Channel MGT
    connect(server,
            SIGNAL(channelConnected(QString)),
            SLOT(channelConnect(QString)));
    connect(server,
            SIGNAL(channelCloseRequest(QString)),
            SLOT(speakerRemove(QString)));
    connect(server,
            SIGNAL(channelDisconnected(QString)),
            SLOT(speakerRemove(QString)));
    // Voting MGT
    connect(server,
            SIGNAL(voteResultsUpdated(VoteResults)),
            SLOT(voteUpdateResults(VoteResults)));
    connect(this,    SIGNAL(voteNew(VotingInvite)),
            server,  SLOT(voteNew(VotingInvite)));
    connect(this,    SIGNAL(voteStop()),
            server,  SLOT(voteStop()));
    // Request MGT
    connect(server,
            SIGNAL(channelRequest(QString)),
            SLOT(channelRequest(QString)));
    connect(this,   SIGNAL(requestAccepted(QString)),
            server, SLOT(channelOpen(QString)));
    connect(this,   SIGNAL(requestDiscarded(QString)),
            server, SLOT(channelDeny(QString)));
    // Load filter settings
    connect(settings, SIGNAL(settingsSaved()),
            server,   SLOT(channelReloadSettings()));
    // Update status
    setStatus(statusStarted);
}

void MainWindow::serverStop()
{
    delete server; server = 0;
    setStatus(statusStoped);
}

void MainWindow::on_addressBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    updateServerInfo();
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
            server->recordStart();
        else
            server->recordStop();
    }
    else
        ui->recordButton->setChecked(false);
}

void MainWindow::on_startVoteButton_toggled(bool checked)
{
    if (!checked)
    {
        emit voteStop();
        ui->startVoteButton->setText(tr("Start"));
    }
    else
    {
        if (ui->simpleRB->isChecked())
            // Simple case
            emit voteNew(VotingInvite(ui->questionTextEdit->toPlainText()));
        else
        {
            QStringList answersText;
            foreach (QLineEdit *l, answers) {
                if (!l->text().isEmpty())
                    answersText.append(l->text());
            }
            // Custom case
            emit voteNew(VotingInvite(ui->questionTextEdit->toPlainText(),
                                      VotingInvite::Custom,
                                      answersText));
        }
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
        ui->resultsLayout->addRow(tr("Yes"), answer);
        answer = new QLabel(tr(resultLabelText)
                            .arg(results.values[1])
                            .arg(results.values[1] * 100.0 / sumAnswers), this);
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
            ui->resultsLayout->addRow(results.invite.answers[i], answer);
        }
    }
    ui->votingDateEdit->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::on_popupResultsButton_toggled(bool checked)
{
    if (checked)
    {
        ui->resultsBox->setParent(resultWidget);
        resultWidget->layout()->addWidget(ui->resultsBox);
        resultWidget->show();
    }
    else
    {
        ui->resultsBox->setParent(this);
        ui->votingTab->layout()->addWidget(ui->resultsBox);
        resultWidget->hide();
    }
}

void MainWindow::on_plusButton_clicked()
{
    QString name = tr("Answer %1:").arg(ui->customLayout->rowCount() + 1);
    answers.append(new QLineEdit(this));
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
