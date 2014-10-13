#ifndef DEBUG_DIALOG_H
#define DEBUG_DIALOG_H

#include <QDialog>
#include "filter.h"

class QTextBrowser;

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

signals:
    void filterSettingsUpdated();

private slots:
    void on_saveButton_clicked();

private:
    Ui::DebugDialog *ui;
    Filter *eq, *hs;
    QString settingsFileName;
};

#endif // DEBUG_DIALOG_H
