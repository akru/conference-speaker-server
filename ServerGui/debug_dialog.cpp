#include "debug_dialog.h"
#include "ui_debug_dialog.h"

#include <hs_filter.h>
#include <equalizer_filter.h>
#include <QSettings>

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog),
    settingsFileName(QApplication::applicationDirPath() + "/settings.ini")
{
    ui->setupUi(this);
    QSettings s(settingsFileName, QSettings::IniFormat);

    ui->eqBox->setChecked(s.value("eq-enable", true).toBool());
    ui->verticalSlider->setValue(s.value("eq-slider-1", 50).toInt());
    ui->verticalSlider_2->setValue(s.value("eq-slider-2", 50).toInt());
    ui->verticalSlider_3->setValue(s.value("eq-slider-3", 50).toInt());
    ui->verticalSlider_4->setValue(s.value("eq-slider-4", 50).toInt());
    ui->verticalSlider_5->setValue(s.value("eq-slider-5", 50).toInt());
    ui->verticalSlider_6->setValue(s.value("eq-slider-6", 50).toInt());
    ui->verticalSlider_7->setValue(s.value("eq-slider-7", 50).toInt());

    ui->hsBox->setChecked(s.value("hs-enable", true).toBool());
    ui->paprSpin->setValue(s.value("hs-papr", 1000).toFloat());
    ui->phprSpin->setValue(s.value("hs-phpr", 1000).toFloat());
    ui->pnprSpin->setValue(s.value("hs-pnpr", 1000).toFloat());
    ui->ismdSpin->setValue(s.value("hs-ismd", 1000).toFloat());

    ui->nsBox->setChecked(s.value("ns-enable", true).toBool());
    QString level = s.value("ns-level", "low").toString();
    if (level == "low")
        ui->lowRadioButton->setChecked(true);
    else
    {
        if (level == "medium")
            ui->mediumRadioButton->setChecked(true);
        else
            ui->highRadioButton->setChecked(true);
    }

    ui->gateBox->setChecked(s.value("gate-enable", true).toBool());
    ui->raiseSpinBox->setValue(s.value("gate-raise", 0.05).toDouble());
    ui->fallSpinBox->setValue(s.value("gate-fall", 0.2).toDouble());
    ui->attackSpinBox->setValue(s.value("gate-attack", 0.1).toDouble());
    ui->holdSpinBox->setValue(s.value("gate-hold", 0.2).toDouble());
    ui->releaseSpinBox->setValue(s.value("gate-release", 0.1).toDouble());

    ui->compressorBox->setChecked(s.value("compressor-enable", true).toBool());
    ui->normalizeCheckBox->setChecked(s.value("compressor-normalize",true).toBool());
    ui->peakCheckBox->setChecked(s.value("compressor-peak", false).toBool());
    ui->cAttackSpinBox->setValue(s.value("compressor-attack", 0.2).toFloat());
    ui->cDecaySpinBox->setValue(s.value("compressor-decay", 1.0).toFloat());
    ui->cRatioSpinBox->setValue(s.value("compressor-ratio", 2).toFloat());
    ui->cCompressionSpinBox->setValue(s.value("compressor-compression", 0.5).toFloat());
    ui->cThresholdSpinBox->setValue(s.value("compressor-threshold", -12).toFloat());
    ui->cNoiseSpinBox->setValue(s.value("compressor-noise", -40).toFloat());

    ui->psBox->setChecked(s.value("ps-enable", true).toBool());
    ui->psCoef->setValue(s.value("ps-coef", 0.04).toFloat());
    ui->psOsamp->setValue(s.value("ps-osamp", 32).toFloat());
}

DebugDialog::~DebugDialog()
{
    delete ui;
}


void DebugDialog::on_saveButton_clicked()
{
    QSettings s(settingsFileName, QSettings::IniFormat);

    s.setValue("eq-enable", ui->eqBox->isChecked());
    s.setValue("eq-slider-1", ui->verticalSlider->value());
    s.setValue("eq-slider-2", ui->verticalSlider_2->value());
    s.setValue("eq-slider-3", ui->verticalSlider_3->value());
    s.setValue("eq-slider-4", ui->verticalSlider_4->value());
    s.setValue("eq-slider-5", ui->verticalSlider_5->value());
    s.setValue("eq-slider-6", ui->verticalSlider_6->value());
    s.setValue("eq-slider-7", ui->verticalSlider_7->value());

    s.setValue("hs-enable", ui->hsBox->isChecked());
    s.setValue("hs-papr", ui->paprEnabled->isChecked() ? ui->paprSpin->value() : 1000);
    s.setValue("hs-phpr", ui->phprEnabled->isChecked() ? ui->phprSpin->value() : 1000);
    s.setValue("hs-pnpr", ui->pnprEnabled->isChecked() ? ui->pnprSpin->value() : 1000);
    s.setValue("hs-ismd", ui->ismdEnabled->isChecked() ? ui->ismdSpin->value() : 0);

    s.setValue("ns-enable", ui->nsBox->isChecked());
    if (ui->lowRadioButton->isChecked())
        s.setValue("ns-level", "low");
    else
    {
        if (ui->mediumRadioButton->isChecked())
            s.setValue("ns-level", "medium");
        else
            s.setValue("ns-level", "high");
    }

    s.setValue("gate-enable",  ui->gateBox->isChecked());
    s.setValue("gate-raise",   ui->raiseSpinBox->value());
    s.setValue("gate-fall",    ui->fallSpinBox->value());
    s.setValue("gate-attack",  ui->attackSpinBox->value());
    s.setValue("gate-hold",    ui->holdSpinBox->value());
    s.setValue("gate-release", ui->releaseSpinBox->value());

    s.setValue("compressor-enable",  ui->compressorBox->isChecked());
    s.setValue("compressor-normalize", ui->normalizeCheckBox->isChecked());
    s.setValue("compressor-peak", ui->peakCheckBox->isChecked());
    s.setValue("compressor-attack", ui->cAttackSpinBox->value());
    s.setValue("compressor-decay", ui->cDecaySpinBox->value());
    s.setValue("compressor-ratio", ui->cRatioSpinBox->value());
    s.setValue("compressor-compression", ui->cCompressionSpinBox->value());
    s.setValue("compressor-threshold", ui->cThresholdSpinBox->value());
    s.setValue("compressor-noise", ui->cNoiseSpinBox->value());

    s.setValue("ps-enable", ui->psBox->isChecked());
    s.setValue("ps-coef", ui->psCoef->value());
    s.setValue("ps-osamp", ui->psOsamp->value());

    emit filterSettingsUpdated();
}
