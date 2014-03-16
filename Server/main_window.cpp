#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    s = new Server(12345);
    connect(s, SIGNAL(userConnected(UserInformation)), SLOT(appendUser(UserInformation)));
    connect(s, SIGNAL(userDisconnected(UserInformation)), SLOT(dropUser(UserInformation)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendUser(UserInformation info)
{
    ui->listWidget->addItem(info.getName());
}

void MainWindow::dropUser(UserInformation info)
{
    QList<QListWidgetItem *> items =
        ui->listWidget->findItems(info.getName(), Qt::MatchFixedString);
    ui->listWidget->removeItemWidget(items[0]);
}
