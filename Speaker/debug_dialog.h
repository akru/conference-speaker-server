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
    explicit DebugDialog(Filter *eq, Filter *hs, QWidget *parent = 0);
    ~DebugDialog();

private slots:
    void on_applyButton_clicked();

private:
    Ui::DebugDialog *ui;
    Filter *eq, *hs;
};

#endif // DEBUG_DIALOG_H
