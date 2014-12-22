#include "settings.h"
#include "ui_main_window.h"
#include "ui_sound_expert_mode.h"
#include "ui_sound_user_mode.h"

#include <QCoreApplication>
#include <QSettings>
#include <QFileDialog>

static const QString defFilename = QStringLiteral("settings-default.ini");

Settings::Settings(Ui::MainWindow *ui,
                   Ui::SoundExpertMode *ex,
                   Ui::SoundUserMode *us,
                   QObject *parent)
    : QObject(parent),
      filename(QCoreApplication::applicationDirPath() + "/settings.ini"),
      ui(ui), ex(ex), us(us)
{
    load();

    connect(ex->eqBox,            SIGNAL(clicked()),        SLOT(save()));
    connect(ex->verticalSlider,   SIGNAL(sliderReleased()), SLOT(save()));
    connect(ex->verticalSlider_2, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ex->verticalSlider_3, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ex->verticalSlider_4, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ex->verticalSlider_5, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ex->verticalSlider_6, SIGNAL(sliderReleased()), SLOT(save()));
    connect(ex->verticalSlider_7, SIGNAL(sliderReleased()), SLOT(save()));

    connect(ex->hsBox,    SIGNAL(clicked()),         SLOT(save()));
    connect(ex->paprSpin, SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->phprSpin, SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->pnprSpin, SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->ismdSpin, SIGNAL(editingFinished()), SLOT(save()));

    connect(ex->nsBox,             SIGNAL(clicked()), SLOT(save()));
    connect(ex->lowRadioButton,    SIGNAL(clicked()), SLOT(save()));
    connect(ex->mediumRadioButton, SIGNAL(clicked()), SLOT(save()));
    connect(ex->highRadioButton,   SIGNAL(clicked()), SLOT(save()));

    connect(ex->gateBox,        SIGNAL(clicked()),         SLOT(save()));
    connect(ex->raiseSpinBox,   SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->fallSpinBox,    SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->attackSpinBox,  SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->holdSpinBox,    SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->releaseSpinBox, SIGNAL(editingFinished()), SLOT(save()));

    connect(ex->compressorBox,       SIGNAL(clicked()),         SLOT(save()));
    connect(ex->normalizeCheckBox,   SIGNAL(clicked()),         SLOT(save()));
    connect(ex->peakCheckBox,        SIGNAL(clicked()),         SLOT(save()));
    connect(ex->cAttackSpinBox,      SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->cDecaySpinBox,       SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->cRatioSpinBox,       SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->cCompressionSpinBox, SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->cThresholdSpinBox,   SIGNAL(editingFinished()), SLOT(save()));
    connect(ex->cNoiseSpinBox,       SIGNAL(editingFinished()), SLOT(save()));

    connect(ex->psBox, SIGNAL(clicked()), SLOT(save()));

    connect(ex->loadSettings, SIGNAL(clicked()), SLOT(loadAsDialog()));
    connect(ex->saveSettings, SIGNAL(clicked()), SLOT(saveAsDialog()));
    connect(ex->loadDefault,  SIGNAL(clicked()), SLOT(loadDefault()));

    connect(us->loadDefaultSettings, SIGNAL(clicked()), SLOT(loadDefault()));

    connect(us->compressionBox, SIGNAL(toggled(bool)),
            ex->compressorBox,  SLOT(setChecked(bool)));
    connect(us->eqPresetBox, SIGNAL(toggled(bool)),
            ex->eqBox, SLOT(setChecked(bool)));
    connect(us->psBox, SIGNAL(toggled(bool)),
            ex->psBox, SLOT(setChecked(bool)));
    connect(us->hsBox, SIGNAL(toggled(bool)),
            ex->hsBox, SLOT(setChecked(bool)));
    connect(us->compressionBox, SIGNAL(toggled(bool)), SLOT(save()));
    connect(us->eqPresetBox, SIGNAL(toggled(bool)),    SLOT(save()));
    connect(us->psBox, SIGNAL(toggled(bool)),          SLOT(save()));
    connect(us->hsBox, SIGNAL(toggled(bool)),          SLOT(save()));

    connect(us->highRadioButton, SIGNAL(clicked()), SLOT(usNoiseControl()));
    connect(us->mediumRadioButton, SIGNAL(clicked()), SLOT(usNoiseControl()));
    connect(us->lowRadioButton, SIGNAL(clicked()), SLOT(usNoiseControl()));
}

void Settings::usNoiseControl()
{
    if (us->highRadioButton->isChecked())
    { // High

    } else if (us->mediumRadioButton->isChecked())
    { // Medium

    } else
    { // Low

    }
    save();
}

ServerInformation Settings::info()
{
    ServerInformation info;
    QSettings s(filename, QSettings::IniFormat);
    info.name    = ui->labelName->text();
    info.address = ui->addressBox->currentText();
    return info;
}

void Settings::load()
{
    loadAs(filename);
}

void Settings::loadAsDialog()
{
    QString fname = QFileDialog::getOpenFileName(this,
                                                 tr("Open preset file"), "",
                                                 tr("Sound filters preset (*.ini)"));
    if (!fname.isEmpty())
        loadAs(fname);
}

void Settings::loadDefault()
{
    loadAs(defFilename);
}

void Settings::save()
{
    saveAs(filename);
}

void Settings::saveAsDialog()
{
    QString fname = QFileDialog::getSaveFileName(this,
                                                 tr("Save preset file"), "",
                                                 tr("Sound filters preset (*.ini)"));
    if (!fname.isEmpty())
        saveAs(fname);
}

void Settings::loadAs(QString fname)
{
    QSettings s(fname, QSettings::IniFormat);
    ui->labelName->setText(s.value("conference-name", "").toString());
    ui->addressBox->setCurrentText(s.value("ip-address", "").toString());

    // Equalizer
    ex->eqBox->setChecked(s.value("eq-enable", true).toBool());
    us->eqBox->setChecked(s.value("eq-enable", true).toBool());

    ex->verticalSlider->setValue(s.value("eq-slider-1", 50).toInt());
    ex->verticalSlider_2->setValue(s.value("eq-slider-2", 50).toInt());
    ex->verticalSlider_3->setValue(s.value("eq-slider-3", 50).toInt());
    ex->verticalSlider_4->setValue(s.value("eq-slider-4", 50).toInt());
    ex->verticalSlider_5->setValue(s.value("eq-slider-5", 50).toInt());
    ex->verticalSlider_6->setValue(s.value("eq-slider-6", 50).toInt());
    ex->verticalSlider_7->setValue(s.value("eq-slider-7", 50).toInt());

    // HS
    ex->hsBox->setChecked(s.value("hs-enable", true).toBool());
    us->hsBox->setChecked(s.value("hs-enable", true).toBool());
    ex->paprSpin->setValue(s.value("hs-papr", 15).toFloat());
    ex->phprSpin->setValue(s.value("hs-phpr", 15).toFloat());
    ex->pnprSpin->setValue(s.value("hs-pnpr", 15).toFloat());
    ex->ismdSpin->setValue(s.value("hs-ismd", 0.03).toFloat());

    // NS
    ex->nsBox->setChecked(s.value("ns-enable", true).toBool());
    QString level = s.value("ns-level", "low").toString();
    if (level == "low")
    {
        ex->lowRadioButton->setChecked(true);
        us->lowRadioButton->setChecked(true);
    } else if (level == "medium")
    {
        ex->mediumRadioButton->setChecked(true);
        us->mediumRadioButton->setChecked(true);
    } else
    {
        ex->highRadioButton->setChecked(true);
        us->highRadioButton->setChecked(true);
    }


    // Gate
    ex->gateBox->setChecked(s.value("gate-enable", true).toBool());
    ex->raiseSpinBox->setValue(s.value("gate-raise", -24.0).toDouble());
    ex->fallSpinBox->setValue(s.value("gate-fall", -28.0).toDouble());
    ex->attackSpinBox->setValue(s.value("gate-attack", 0.1).toDouble());
    ex->holdSpinBox->setValue(s.value("gate-hold", 0.2).toDouble());
    ex->releaseSpinBox->setValue(s.value("gate-release", 0.1).toDouble());

    // Compressor
    ex->compressorBox->setChecked(s.value("compressor-enable", true).toBool());
    us->compressorBox->setChecked(s.value("compressor-enable", true).toBool());

    ex->normalizeCheckBox->setChecked(s.value("compressor-normalize",true).toBool());
    ex->peakCheckBox->setChecked(s.value("compressor-peak", false).toBool());
    ex->cAttackSpinBox->setValue(s.value("compressor-attack", 0.2).toFloat());
    ex->cDecaySpinBox->setValue(s.value("compressor-decay", 1.0).toFloat());
    ex->cRatioSpinBox->setValue(s.value("compressor-ratio", 2).toFloat());
    ex->cCompressionSpinBox->setValue(s.value("compressor-compression", 0.5).toFloat());
    ex->cThresholdSpinBox->setValue(s.value("compressor-threshold", -12).toFloat());
    ex->cNoiseSpinBox->setValue(s.value("compressor-noise", -40).toFloat());

    // Pitch shift
    ex->psBox->setChecked(s.value("ps-enable", true).toBool());
    us->psBox->setChecked(s.value("ps-enable", true).toBool());

    // Records storage
    ui->storageEdit->setText(s.value("records-storage", "").toString());
}

void Settings::saveAs(QString fname)
{
    QSettings s(fname, QSettings::IniFormat);
    s.setValue("conference-name", ui->labelName->text());
    s.setValue("ip-address", ui->addressBox->currentText());

    s.setValue("eq-enable", ex->eqBox->isChecked());
    s.setValue("eq-slider-1", ex->verticalSlider->value());
    s.setValue("eq-slider-2", ex->verticalSlider_2->value());
    s.setValue("eq-slider-3", ex->verticalSlider_3->value());
    s.setValue("eq-slider-4", ex->verticalSlider_4->value());
    s.setValue("eq-slider-5", ex->verticalSlider_5->value());
    s.setValue("eq-slider-6", ex->verticalSlider_6->value());
    s.setValue("eq-slider-7", ex->verticalSlider_7->value());

    s.setValue("hs-enable", ex->hsBox->isChecked());
    s.setValue("hs-papr", ex->paprEnabled->isChecked() ? ex->paprSpin->value() : 15.0);
    s.setValue("hs-phpr", ex->phprEnabled->isChecked() ? ex->phprSpin->value() : 15.0);
    s.setValue("hs-pnpr", ex->pnprEnabled->isChecked() ? ex->pnprSpin->value() : 15.0);
    s.setValue("hs-ismd", ex->ismdEnabled->isChecked() ? ex->ismdSpin->value() : 0.03);

    s.setValue("ns-enable", ex->nsBox->isChecked());
    if (ex->lowRadioButton->isChecked())
        s.setValue("ns-level", "low");
    else
    {
        if (ex->mediumRadioButton->isChecked())
            s.setValue("ns-level", "medium");
        else
            s.setValue("ns-level", "high");
    }

    s.setValue("gate-enable",  ex->gateBox->isChecked());
    s.setValue("gate-raise",   ex->raiseSpinBox->value());
    s.setValue("gate-fall",    ex->fallSpinBox->value());
    s.setValue("gate-attack",  ex->attackSpinBox->value());
    s.setValue("gate-hold",    ex->holdSpinBox->value());
    s.setValue("gate-release", ex->releaseSpinBox->value());

    s.setValue("compressor-enable",  ex->compressorBox->isChecked());
    s.setValue("compressor-normalize", ex->normalizeCheckBox->isChecked());
    s.setValue("compressor-peak", ex->peakCheckBox->isChecked());
    s.setValue("compressor-attack", ex->cAttackSpinBox->value());
    s.setValue("compressor-decay", ex->cDecaySpinBox->value());
    s.setValue("compressor-ratio", ex->cRatioSpinBox->value());
    s.setValue("compressor-compression", ex->cCompressionSpinBox->value());
    s.setValue("compressor-threshold", ex->cThresholdSpinBox->value());
    s.setValue("compressor-noise", ex->cNoiseSpinBox->value());

    s.setValue("ps-enable", ex->psBox->isChecked());

    s.setValue("records-storage", ui->storageEdit->text());

    emit settingsSaved();
}
