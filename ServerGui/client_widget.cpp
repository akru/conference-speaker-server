#include "client_widget.h"
#include "ui_client_widget.h"

ClientWidget::ClientWidget(UserInformation &user,
                           QString &address,
                           QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ClientWidget),
      address(address)
{
    ui->setupUi(this);
    ui->nameLabel->setText(user.name);
    ui->companyLabel->setText(user.company);
}

ClientWidget::~ClientWidget()
{
    delete ui;
}
