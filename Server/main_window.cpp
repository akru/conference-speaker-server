#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    s = new Server(QHostAddress::Any, 12345);
    connect(s, SIGNAL(userConnected(QString,UserInformation)),
            SLOT(appendUser(QString,UserInformation)));
    connect(s, SIGNAL(userDisconnected(QString,UserInformation)),
            SLOT(dropUser(QString,UserInformation)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendUser(QString address, UserInformation info)
{
    ui->listWidget->addItem(address + " :: " + info.name);
}

void MainWindow::dropUser(QString address, UserInformation info)
{
    Q_UNUSED(address)
    Q_UNUSED(info)
}
