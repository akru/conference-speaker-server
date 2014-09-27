#include "debug_dialog.h"
#include "ui_debug_dialog.h"

#include <hs_filter.h>
#include <equalizer_filter.h>
#include <QSettings>

DebugDialog::DebugDialog(Filter *eq, Filter *hs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog), eq(eq), hs(hs),
    settingsFileName(QApplication::applicationDirPath() + "/settings.ini")
{
    ui->setupUi(this);
    connect(ui->verticalSlider, SIGNAL(sliderReleased()), SLOT(on_applyButton_clicked()));
    connect(ui->verticalSlider_2, SIGNAL(sliderReleased()), SLOT(on_applyButton_clicked()));
    connect(ui->verticalSlider_3, SIGNAL(sliderReleased()), SLOT(on_applyButton_clicked()));
    connect(ui->verticalSlider_4, SIGNAL(sliderReleased()), SLOT(on_applyButton_clicked()));
    connect(ui->verticalSlider_5, SIGNAL(sliderReleased()), SLOT(on_applyButton_clicked()));
    connect(ui->verticalSlider_6, SIGNAL(sliderReleased()), SLOT(on_applyButton_clicked()));
    connect(ui->verticalSlider_7, SIGNAL(sliderReleased()), SLOT(on_applyButton_clicked()));

    QSettings s(settingsFileName, QSettings::IniFormat);
    ui->verticalSlider->setValue(s.value("eq-slider-1", 50).toInt());
    ui->verticalSlider_2->setValue(s.value("eq-slider-2", 50).toInt());
    ui->verticalSlider_3->setValue(s.value("eq-slider-3", 50).toInt());
    ui->verticalSlider_4->setValue(s.value("eq-slider-4", 50).toInt());
    ui->verticalSlider_5->setValue(s.value("eq-slider-5", 50).toInt());
    ui->verticalSlider_6->setValue(s.value("eq-slider-6", 50).toInt());
    ui->verticalSlider_7->setValue(s.value("eq-slider-7", 50).toInt());
    ui->paprSpin->setValue(s.value("hs-papr", 1000).toFloat());
    ui->phprSpin->setValue(s.value("hs-phpr", 1000).toFloat());
    ui->pnprSpin->setValue(s.value("hs-pnpr", 1000).toFloat());
    ui->ismdSpin->setValue(s.value("hs-ismd", 1000).toFloat());
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
    eqf->setUserBand(ui->verticalSlider->value(),
                     ui->verticalSlider_2->value(),
                     ui->verticalSlider_3->value(),
                     ui->verticalSlider_4->value(),
                     ui->verticalSlider_5->value(),
                     ui->verticalSlider_6->value(),
                     ui->verticalSlider_7->value());

    QSettings s(settingsFileName, QSettings::IniFormat);
    s.setValue("eq-slider-1", ui->verticalSlider->value());
    s.setValue("eq-slider-2", ui->verticalSlider_2->value());
    s.setValue("eq-slider-3", ui->verticalSlider_3->value());
    s.setValue("eq-slider-4", ui->verticalSlider_4->value());
    s.setValue("eq-slider-5", ui->verticalSlider_5->value());
    s.setValue("eq-slider-6", ui->verticalSlider_6->value());
    s.setValue("eq-slider-7", ui->verticalSlider_7->value());
    s.setValue("hs-papr", ui->paprSpin->value());
    s.setValue("hs-phpr", ui->phprSpin->value());
    s.setValue("hs-pnpr", ui->pnprSpin->value());
    s.setValue("hs-ismd", ui->ismdSpin->value());
}
