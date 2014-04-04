#include "main_window.h"
#include "ui_main_window.h"
#include <QMessageBox>
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(0)
{
    ui->setupUi(this);
    // Restart server with new info
    connect(&settings, SIGNAL(newServerInfo(ServerInformation)),
            SLOT(updateServerInfo(ServerInformation)));
    // Reload broadcaster server information
    connect(&settings, SIGNAL(newServerInfo(ServerInformation)),
            &broadcaster, SLOT(setServerInformation(ServerInformation)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendUser(QString address, UserInformation info)
{
    QString userDescription = info.name + " : " + info.company + " : "
            + info.title + " (" + address + ")";
    ui->userList->addItem(userDescription);
}

void MainWindow::dropUser(QString address)
{
    delete ui->userList->findItems(address, Qt::MatchContains)[0];
}

void MainWindow::appendChannel(QString address, UserInformation info, Receiver *channel)
{
    ChannelWidget *c = new ChannelWidget(address, info, channel, this);
    channels.insert(address, c);

    ui->channelsBox->layout()->addWidget(c);
    c->show();

    connect(c, SIGNAL(closeChannelClicked(QString)),
            server, SLOT(closeChannel(QString)));
    connect(c, SIGNAL(closeChannelClicked(QString)),
            SLOT(dropChannel(QString)));
}

void MainWindow::dropChannel(QString address)
{
    Q_ASSERT(channels.contains(address));
    channels[address]->close();
    delete channels[address];
    channels.remove(address);
}

void MainWindow::channelRequest(Connection *client, UserInformation info)
{
    QMessageBox msgBox;
    msgBox.setText(tr("Speaker <b>") + info.name + tr("</b> want to tell."));
    msgBox.setInformativeText(tr("Do you want to permit?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:
        emit channelRequestAccepted(client);
        break;
    case QMessageBox::Cancel:
        emit channelRequestDiscarded(client);
        break;
    }
}

void MainWindow::updateServerInfo(ServerInformation info)
{
    delete server;
    server = new Server(info.address);

    connect(server, SIGNAL(userConnected(QString,UserInformation)),
            SLOT(appendUser(QString,UserInformation)));
    connect(server, SIGNAL(userDisconnected(QString)),
            SLOT(dropUser(QString)));

    connect(server, SIGNAL(channelConnected(QString,UserInformation,Receiver*)),
            SLOT(appendChannel(QString,UserInformation,Receiver*)));
    connect(server, SIGNAL(channelDisconnected(QString)),
            SLOT(dropChannel(QString)));

    connect(server, SIGNAL(voteRequest(Connection*,bool)),
            &voting, SLOT(vote(Connection*,bool)));
    connect(&voting, SIGNAL(voteAccepted(Connection*)),
            server, SLOT(acceptVote(Connection*)));
    connect(&voting, SIGNAL(voteDenied(Connection*)),
            server, SLOT(denyVote(Connection*)));

    connect(server, SIGNAL(channelRequest(Connection*,UserInformation)),
            SLOT(channelRequest(Connection*,UserInformation)));
    connect(this, SIGNAL(channelRequestAccepted(Connection*)),
            server, SLOT(openChannel(Connection*)));
    connect(this, SIGNAL(channelRequestDiscarded(Connection*)),
            server, SLOT(denyChannel(Connection*)));
}

void MainWindow::on_actionAbout_triggered()
{
    about.show();
}

void MainWindow::on_actionSettings_triggered()
{
    settings.show();
}

void MainWindow::on_voteButton_clicked()
{
    voting.newVote(ui->userList->count());
    voting.show();
}
