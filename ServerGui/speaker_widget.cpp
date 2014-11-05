#include "speaker_widget.h"
#include "ui_speaker_widget.h"
#include <QTimer>

SpeakerWidget::SpeakerWidget(const QString &address,
                             const UserInformation &info,
                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpeakerWidget),
    myAddress(address)
{
    ui->setupUi(this);
    ui->nameLabel->setText(info.name);
    ui->titleLabel->setText(info.title);
    ui->companyLabel->setText(info.company);
    setState(Request);

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), ui->delayBox, SLOT(stepUp()));
    t->setInterval(1000);
    t->start();
}

SpeakerWidget::~SpeakerWidget()
{
    delete ui;
}

void SpeakerWidget::setState(State s)
{
    switch (s) {
    case Request:
        ui->volumeFrame->hide();
        ui->titleLabel->hide();
        ui->companyLabel->hide();
        ui->acceptButton->show();
        ui->delayBox->show();
        setMaximumHeight(50);
        break;
    case Stream:
        ui->volumeFrame->show();
        ui->titleLabel->show();
        ui->companyLabel->show();
        ui->acceptButton->hide();
        ui->delayBox->hide();
        setMaximumHeight(130);
        break;
    }
    state = s;
}

void SpeakerWidget::on_acceptButton_clicked()
{
    emit requestAccepted(myAddress);
}

void SpeakerWidget::on_closeButton_clicked()
{
    switch (state) {
    case Request:
        emit requestDiscarded(myAddress);
        break;
    case Stream:
        emit closeClicked(myAddress);
        break;
    }
}

void SpeakerWidget::setAmplitude(QString address, ushort amp)
{
    if (address == myAddress)
        ui->volumeBar->setValue(amp);
}

void SpeakerWidget::on_volumeSlider_sliderMoved(int position)
{
    emit volumeChanged(myAddress, position / 100.0);
}
