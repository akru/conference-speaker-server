#include "settings.h"
#include "ui_main_window.h"

#include <QCoreApplication>
#include <QSettings>

Settings::Settings(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent),
      filename(QCoreApplication::applicationDirPath() + "/settings.ini"),
      ui(ui)
{
    QSettings s(filename, QSettings::IniFormat);
    info.name    = s.value("conference-name", "").toString();
    info.address = s.value("ip-address", "").toString();

    // Equalizer
    ui->eqBox->setChecked(s.value("eq-enable", true).toBool());
    ui->verticalSlider->setValue(s.value("eq-slider-1", 50).toInt());
    ui->verticalSlider_2->setValue(s.value("eq-slider-2", 50).toInt());
    ui->verticalSlider_3->setValue(s.value("eq-slider-3", 50).toInt());
    ui->verticalSlider_4->setValue(s.value("eq-slider-4", 50).toInt());
    ui->verticalSlider_5->setValue(s.value("eq-slider-5", 50).toInt());
    ui->verticalSlider_6->setValue(s.value("eq-slider-6", 50).toInt());
    ui->verticalSlider_7->setValue(s.value("eq-slider-7", 50).toInt());

    connect(ui->eqBox, SIGNAL(clicked()), SLOT(save()));
    connect(ui->verticalSlider, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ui->verticalSlider_2, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ui->verticalSlider_3, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ui->verticalSlider_4, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ui->verticalSlider_5, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ui->verticalSlider_6, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ui->verticalSlider_7, SIGNAL(sliderReleased()), SLOT(save()));

    // HS
    ui->hsBox->setChecked(s.value("hs-enable", true).toBool());
    ui->paprSpin->setValue(s.value("hs-papr", 1000).toFloat());
    ui->phprSpin->setValue(s.value("hs-phpr", 1000).toFloat());
    ui->pnprSpin->setValue(s.value("hs-pnpr", 1000).toFloat());
    ui->ismdSpin->setValue(s.value("hs-ismd", 1000).toFloat());

    connect(ui->hsBox, SIGNAL(clicked()), SLOT(save()));
    connect(ui->paprSpin, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->phprSpin, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->pnprSpin, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->ismdSpin, SIGNAL(editingFinished()), SLOT(save()));

    // NS
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

    connect(ui->nsBox, SIGNAL(clicked()), SLOT(save()));
    connect(ui->lowRadioButton, SIGNAL(clicked()), SLOT(save()));
    connect(ui->mediumRadioButton, SIGNAL(clicked()), SLOT(save()));
    connect(ui->highRadioButton, SIGNAL(clicked()), SLOT(save()));

    // Gate
    ui->gateBox->setChecked(s.value("gate-enable", true).toBool());
    ui->raiseSpinBox->setValue(s.value("gate-raise", 0.05).toDouble());
    ui->fallSpinBox->setValue(s.value("gate-fall", 0.2).toDouble());
    ui->attackSpinBox->setValue(s.value("gate-attack", 0.1).toDouble());
    ui->holdSpinBox->setValue(s.value("gate-hold", 0.2).toDouble());
    ui->releaseSpinBox->setValue(s.value("gate-release", 0.1).toDouble());

    connect(ui->nsBox, SIGNAL(clicked()), SLOT(save()));
    connect(ui->raiseSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->fallSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->attackSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->holdSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->releaseSpinBox, SIGNAL(editingFinished()), SLOT(save()));

    // Compressor
    ui->compressorBox->setChecked(s.value("compressor-enable", true).toBool());
    ui->normalizeCheckBox->setChecked(s.value("compressor-normalize",true).toBool());
    ui->peakCheckBox->setChecked(s.value("compressor-peak", false).toBool());
    ui->cAttackSpinBox->setValue(s.value("compressor-attack", 0.2).toFloat());
    ui->cDecaySpinBox->setValue(s.value("compressor-decay", 1.0).toFloat());
    ui->cRatioSpinBox->setValue(s.value("compressor-ratio", 2).toFloat());
    ui->cCompressionSpinBox->setValue(s.value("compressor-compression", 0.5).toFloat());
    ui->cThresholdSpinBox->setValue(s.value("compressor-threshold", -12).toFloat());
    ui->cNoiseSpinBox->setValue(s.value("compressor-noise", -40).toFloat());

    connect(ui->nsBox, SIGNAL(clicked()), SLOT(save()));
    connect(ui->normalizeCheckBox, SIGNAL(clicked()), SLOT(save()));
    connect(ui->peakCheckBox, SIGNAL(clicked()), SLOT(save()));
    connect(ui->cAttackSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->cDecaySpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->cRatioSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->cCompressionSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->cThresholdSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ui->cNoiseSpinBox, SIGNAL(editingFinished()), SLOT(save()));

    // Pitch shift
    ui->psBox->setChecked(s.value("ps-enable", true).toBool());
    connect(ui->psBox, SIGNAL(clicked()), SLOT(save()));
}

void Settings::save()
{
    QSettings s(filename, QSettings::IniFormat);
    s.setValue("conference-name", info.name);
    s.setValue("ip-address", info.address);

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

    emit settingsSaved();
}
