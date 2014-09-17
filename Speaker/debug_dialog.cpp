#include "debug_dialog.h"
#include "ui_debug_dialog.h"

#include <hs_filter.h>
#include <equalizer_filter.h>

DebugDialog::DebugDialog(Filter *eq, Filter *hs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog), eq(eq), hs(hs)
{
    ui->setupUi(this);
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::on_applyButton_clicked()
{
    HSFilter *hsf = (HSFilter *) hs;
    hsf->setTH(ui->paprSpin->value(),
               ui->phprSpin->value(),
               ui->pnprSpin->value(),
               ui->ismdSpin->value());
    EqualizerFilter *eqf = (EqualizerFilter *) eq;
    eqf->setSixBand(ui->verticalSlider->value(),
                    ui->verticalSlider_2->value(),
                    ui->verticalSlider_3->value(),
                    ui->verticalSlider_4->value(),
                    ui->verticalSlider_5->value(),
                    ui->verticalSlider_6->value());
}
