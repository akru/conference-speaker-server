#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include <QNetworkInterface>
#include <QSettings>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    configured(false),
    addrList(QNetworkInterface::allAddresses())
{
    ui->setupUi(this);

    settingsFileName =
            QApplication::applicationDirPath() + "/settings.ini";
    qDebug() << "Settings file:" << settingsFileName;

    foreach (const QHostAddress &address, addrList) {
        ui->addressSelect->addItem(address.toString());
    }
    connect(this, SIGNAL(accepted()), SLOT(genServerInfo()));

    loadSettings();
    if (!configured)
        show();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::genServerInfo()
{
    QString name = ui->nameEdit->text(),
            addr = ui->addressSelect->currentText();
    info = ServerInformation(name, addr);
    qDebug() << "New server info" << info.toJson();

    QSettings s(settingsFileName, QSettings::NativeFormat);
    s.setValue("conference-name", name);
    s.setValue("ip-address", addr);

    emit newServerInfo(info);
}

void SettingsDialog::loadSettings()
{
    QSettings s(settingsFileName, QSettings::NativeFormat);
    QString name = s.value("conference-name", "").toString(),
            addr = s.value("ip-address", "").toString();

    if (!name.isEmpty() && !addr.isEmpty())
    {
        configured = true;
        ui->nameEdit->setText(name);
        int addrId = ui->addressSelect->findText(addr);
        ui->addressSelect->setCurrentIndex(addrId);

        genServerInfo();
    }
}
