#ifndef DEBUG_DIALOG_H
#define DEBUG_DIALOG_H

#include <QDialog>

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
    QTextBrowser * textBrowser() const;

signals:
    void trasholdes(qreal, qreal, qreal, qreal);

private slots:
    void on_pushButton_clicked();

private:
    Ui::DebugDialog *ui;
};

#endif // DEBUG_DIALOG_H
