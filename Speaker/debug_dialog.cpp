#include "debug_dialog.h"
#include "ui_debug_dialog.h"

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

QTextBrowser * DebugDialog::textBrowser() const
{
    return ui->textBrowser;
}

void DebugDialog::on_pushButton_clicked()
{
    emit trasholdes(ui->paprEnabled->isChecked() ? ui->paprSpin->value() : -1000,
                    ui->phprEnabled->isChecked() ? ui->phprSpin->value() : -1000,
                    ui->pnprEnabled->isChecked() ? ui->pnprSpin->value() : -1000,
                    ui->ismdEnabled->isChecked() ? ui->ismdSpin->value() : 1000);
}
