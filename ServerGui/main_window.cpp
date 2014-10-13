#include "main_window.h"
#include "ui_main_window.h"
#include <QMessageBox>
#include <QHostAddress>
#include <QFontDatabase>
#include <QFile>

const QString clientsHeader = "Clients: <b style=\"color: #00A0E3\">(%1)</b>";
const QString wantsHeader = "Wants to ask: <b style=\"color: #00A0E3\">(%1)</b>";
const QString chatHeader = "Chat: <b style=\"color: #00A0E3\">(%1)</b>";

const QString statusBarNonConfig = "Not configured, please set a settings by settings dialog";
const QString statusBarConfigured = "Configured, waiting for clients...";
const QString statusBarConnected = "Working, connected %1 clients";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(0)
{
    // Loading fonts
    QFontDatabase::addApplicationFont(":/res/fonts/BauhausC-Demibold.ttf");
    //QFontDatabase::addApplicationFont(":/res/fonts/Gothic.ttf");
    QFontDatabase::addApplicationFont(":/res/fonts/GothicBold.ttf");
    // Setup UI
    ui->setupUi(this);
    // Setup voting
    ui->voteBox->layout()->addWidget(&voting);
    voting.show();
    // Restart server with new info
    connect(&settings, SIGNAL(newServerInfo(ServerInformation)),
            SLOT(updateServerInfo(ServerInformation)));
    // Reload broadcaster server information
    connect(&settings, SIGNAL(newServerInfo(ServerInformation)),
            &broadcaster, SLOT(setServerInformation(ServerInformation)));
    // Load server settings
    settings.loadSettings();
    // Set scroll boxes alignments
    ui->channelBox->setAlignment(Qt::AlignTop);
    ui->wantsBox->layout()->setAlignment(Qt::AlignTop);
    ui->clientBox->layout()->setAlignment(Qt::AlignTop);
    // Update headers
    ui->clientLabel->setText(clientsHeader.arg(0));
    ui->wantsLabel->setText(wantsHeader.arg(0));
    ui->chatLabel->setText(chatHeader.arg(0));
    if (settings.isConfigured())
    {
        ui->labelName->setText(settings.serverInfo().name);
        ui->statusBar->showMessage(statusBarConfigured);
    }
    else
    {
        ui->statusBar->showMessage(statusBarNonConfig);
        settings.show();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendClient(QString address, UserInformation info)
{
    Q_ASSERT(!clients.contains(address));

    ClientWidget *w = new ClientWidget(info, address, this);
    ui->clientBox->layout()->addWidget(w);
    clients.insert(address, w);
    w->show();
    // Ban handler
    connect(w, SIGNAL(banned(QString)), server, SLOT(dropUser(QString)));
    // Update vote info
    voting.appendClient();
    // Update header
    int countClients = ui->clientBox->layout()->count();
    ui->clientLabel->setText(clientsHeader.arg(countClients));
    ui->statusBar->showMessage(statusBarConnected.arg(countClients));
}

void MainWindow::dropClient(QString address)
{
    Q_ASSERT(clients.contains(address));
    delete clients[address];
    clients.remove(address);

    if (requests.contains(address))
    {
        dropRequest(address);
    }
    if (channels.contains(address))
    {
        dropChannel(address);
    }
    // Update vote info
    voting.dropClient();
    // Update header
    int countClients = ui->clientBox->layout()->count();
    ui->clientLabel->setText(clientsHeader.arg(countClients));
    ui->statusBar->showMessage(statusBarConnected.arg(countClients));
}

void MainWindow::appendChannel(QString address, UserInformation info, Receiver *channel)
{
    ChannelWidget *c = new ChannelWidget(address, info, channel, this);
    channels.insert(address, c);

    ui->channelBox->addWidget(c);
    c->show();

    connect(c, SIGNAL(closeChannelClicked(QString)),
            server, SLOT(closeChannel(QString)));
    connect(c, SIGNAL(closeChannelClicked(QString)),
            SLOT(dropChannel(QString)));
    // Update header
    ui->chatLabel->setText(
                chatHeader.arg(ui->channelBox->count()));
}

void MainWindow::dropChannel(QString address)
{
    Q_ASSERT(channels.contains(address));
    channels[address]->close();
    delete channels[address];
    channels.remove(address);
    // Update header
    ui->chatLabel->setText(
                chatHeader.arg(ui->channelBox->count()));
}

void MainWindow::channelRequest(QString address, UserInformation info)
{
    if(requests.contains(address))
        return;

    RequestWidget *reqWidget = new RequestWidget(info, address, this);
    requests[address] = reqWidget;
    connect(reqWidget, SIGNAL(accepted(QString)),
            server, SLOT(openChannel(QString)));
    connect(reqWidget, SIGNAL(discarded(QString)),
            server, SLOT(denyChannel(QString)));
    connect(reqWidget, SIGNAL(accepted(QString)),
            SLOT(dropRequest(QString)));
    connect(reqWidget, SIGNAL(discarded(QString)),
            SLOT(dropRequest(QString)));

    ui->wantsBox->layout()->addWidget(reqWidget);
    reqWidget->show();
    // Update header
    ui->wantsLabel->setText(
                wantsHeader.arg(ui->wantsBox->layout()->count()));
}

void MainWindow::dropRequest(QString address)
{
    if (requests.contains(address))
    {
        requests[address]->close();
        delete requests[address];
        requests.remove(address);
        // Update header
        ui->wantsLabel->setText(
                    wantsHeader.arg(ui->wantsBox->layout()->count()));
    }
}

void MainWindow::updateServerInfo(ServerInformation info)
{
    delete server;
    server = new Server(info.address);

    connect(server, SIGNAL(userConnected(QString,UserInformation)),
            SLOT(appendClient(QString,UserInformation)));
    connect(server, SIGNAL(userDisconnected(QString)),
            SLOT(dropClient(QString)));

    connect(server, SIGNAL(channelConnected(QString,UserInformation,Receiver*)),
            SLOT(appendChannel(QString,UserInformation,Receiver*)));
    connect(server, SIGNAL(channelCloseRequest(QString)),
            SLOT(dropRequest(QString)));
    connect(server, SIGNAL(channelDisconnected(QString)),
            SLOT(dropChannel(QString)));

    connect(server, SIGNAL(voteRequest(QString,bool)),
            &voting, SLOT(vote(QString,bool)));
    connect(&voting, SIGNAL(voteAccepted(QString)),
            server, SLOT(acceptVote(QString)));
    connect(&voting, SIGNAL(voteDenied(QString)),
            server, SLOT(denyVote(QString)));

    connect(server, SIGNAL(channelRequest(QString,UserInformation)),
            SLOT(channelRequest(QString,UserInformation)));
    connect(this, SIGNAL(channelRequestAccepted(QString)),
            server, SLOT(openChannel(QString)));
    connect(this, SIGNAL(channelRequestDiscarded(QString)),
            server, SLOT(denyChannel(QString)));

    connect(&filter_setup, SIGNAL(filterSettingsUpdated()),
            server,        SLOT(reloadFilterSettings()));

    // Update headers
    ui->labelName->setText(info.name);
    ui->statusBar->showMessage(statusBarConfigured);
}

void MainWindow::on_actionAbout_triggered()
{
    about.show();
}

void MainWindow::on_actionSettings_triggered()
{
    settings.show();
}

void MainWindow::on_actionSound_processing_triggered()
{
    filter_setup.show();
}
