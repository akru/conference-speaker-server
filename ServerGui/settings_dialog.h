#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QHostAddress>
#include <server_information.h>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    void loadSettings();

    bool isConfigured() const { return configured; }
    ServerInformation serverInfo() const { return info; }

signals:
    void newServerInfo(ServerInformation info);

private slots:
    void genServerInfo();

private:
    Ui::SettingsDialog *ui;
    bool configured;
    QString settingsFileName;
    ServerInformation info;

    QList<QHostAddress> addrList;
};

#endif // SETTINGS_DIALOG_H
