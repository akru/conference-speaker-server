#include "request_widget.h"
#include "ui_request_widget.h"

RequestWidget::RequestWidget(UserInformation &user, Connection *client, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::RequestWidget),
      client(client)
{
    ui->setupUi(this);
    ui->nameLabel->setText(user.name);
    ui->companyLabel->setText(user.company);
}

RequestWidget::~RequestWidget()
{
    delete ui;
}

