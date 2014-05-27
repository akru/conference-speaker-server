#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include <QNetworkInterface>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    addrList(QNetworkInterface::allAddresses())
{
    ui->setupUi(this);

    foreach (const QHostAddress &address, addrList) {
        ui->addressSelect->addItem(address.toString());
    }
    connect(this, SIGNAL(accepted()), SLOT(genServerInfo()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::genServerInfo()
{
    ServerInformation info(ui->nameEdit->text(),
                           ui->addressSelect->currentText());
    qDebug() << "New server info" << info.toJson();
    emit newServerInfo(info);
}
