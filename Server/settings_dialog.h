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

signals:
    void newServerInfo(ServerInformation info);

private slots:
    void genServerInfo();

private:
    Ui::SettingsDialog *ui;
    QList<QHostAddress> addrList;
};

#endif // SETTINGS_DIALOG_H
