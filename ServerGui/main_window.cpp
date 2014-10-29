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
    settings(new Settings),
    server(0)
{
    // Loading fonts
    loadFonts();
    // Setup UI
    ui->setupUi(this);
    // Setup UI elements
    setupUi();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    Q_ASSERT(!userItem.contains(address));
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

    // TODO: Boost widget to top

    w->setState(SpeakerWidget::Stream);
}

void MainWindow::speakerRemove(QString address)
{
    Q_ASSERT(speakers.contains(address));
    delete speakers.take(address);
}

void MainWindow::channelRequest(QString address)
{
    Q_ASSERT(!speakers.contains(address));

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

//    connect(server,  SIGNAL(voteResultsUpdated(VoteResults)),
//            &voting, SLOT(updateResults(VoteResults)));
//    connect(&voting, SIGNAL(voteNew(VotingInvite)),
//            server,  SLOT(voteNew(VotingInvite)));
//    connect(&voting, SIGNAL(voteStop()),
//            server,  SLOT(voteStop()));
    // Request MGT
    connect(server,
            SIGNAL(channelRequest(QString)),
            SLOT(channelRequest(QString)));
    connect(this,   SIGNAL(requestAccepted(QString)),
            server, SLOT(channelOpen(QString)));
    connect(this,   SIGNAL(requestDiscarded(QString)),
            server, SLOT(channelDeny(QString)));

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

