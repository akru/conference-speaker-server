#include "main_window.h"
#include "ui_main_window.h"
#include "server.h"

MainWindow::MainWindow(Server *server, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(server)
{
    ui->setupUi(this);

    connect(server, SIGNAL(userConnected(QString,UserInformation)),
            SLOT(appendUser(QString,UserInformation)));
    connect(server, SIGNAL(userDisconnected(QString,UserInformation)),
            SLOT(dropUser(QString,UserInformation)));

    // TODO: change
    connect(server, SIGNAL(channelRequest(Connection*)),
            server, SLOT(openChannel(Connection*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendUser(QString address, UserInformation info)
{
    ui->listWidget->addItem(info.name + " (" + address + ")");
}

void MainWindow::dropUser(QString address, UserInformation info)
{
    Q_UNUSED(info)
    delete ui->listWidget->findItems(address, Qt::MatchContains)[0];
}

void MainWindow::on_actionAbout_triggered()
{
    about.show();
}
