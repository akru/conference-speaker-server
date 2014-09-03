#include "request_widget.h"
#include "ui_request_widget.h"

RequestWidget::RequestWidget(UserInformation &user,
                             QString address,
                             QWidget *parent)
    : QWidget(parent),
      ui(new Ui::RequestWidget),
      client(address)
{
    ui->setupUi(this);
    ui->nameLabel->setText(user.name);
    ui->companyLabel->setText(user.company);
}

RequestWidget::~RequestWidget()
{
    delete ui;
}

