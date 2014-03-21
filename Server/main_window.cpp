#include "main_window.h"
#include "ui_main_window.h"
#include "channel_widget.h"
#include <QMessageBox>

MainWindow::MainWindow(Server *server, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(server)
{
    ui->setupUi(this);

    connect(server, SIGNAL(userConnected(QString,UserInformation)),
            SLOT(appendUser(QString,UserInformation)));
    connect(server, SIGNAL(userDisconnected(QString)),
            SLOT(dropUser(QString)));

    connect(server, SIGNAL(channelConnected(UserInformation,Receiver*)),
                    SLOT(appendChannel(UserInformation,Receiver*)));

    connect(server, SIGNAL(channelRequest(Connection*,UserInformation)),
            SLOT(channelRequest(Connection*,UserInformation)));
    connect(this, SIGNAL(channelRequestAccepted(Connection*)),
            server, SLOT(openChannel(Connection*)));
    connect(this, SIGNAL(channelRequestDiscarded(Connection*)),
            server, SLOT(denyChannel(Connection*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendUser(QString address, UserInformation info)
{
    ui->userList->addItem(info.name + " (" + address + ")");
}

void MainWindow::dropUser(QString address)
{
    delete ui->userList->findItems(address, Qt::MatchContains)[0];
}

void MainWindow::appendChannel(UserInformation info, Receiver *channel)
{
    ChannelWidget *c = new ChannelWidget(info, channel, this);
    ui->channelsBox->layout()->addWidget(c);
    c->show();
    connect(channel, SIGNAL(disconnected(Receiver*)),
            c, SLOT(close()));
    connect(c, SIGNAL(closeChannelClicked(QString)),
            server, SLOT(closeChannel(QString)));
}

void MainWindow::channelRequest(Connection *client, UserInformation info)
{
    QMessageBox msgBox;
    msgBox.setText("Speaker <b>" + info.name + "</b> want to tell.");
    msgBox.setInformativeText("Do you want to permit?");
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

void MainWindow::on_actionAbout_triggered()
{
    about.show();
}
