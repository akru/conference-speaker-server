#include "speaker_widget.h"
#include "ui_speaker_widget.h"
#include <QTimer>
#include <speaker.h>

SpeakerWidget::SpeakerWidget(User *user, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::SpeakerWidget),
      user(user),
      stepVal(0)
{
    ui->setupUi(this);
    ui->nameLabel->setText(user->getInfo().name.isEmpty() ? tr("Anonymous") : user->getInfo().name);
    ui->titleLabel->setText(user->getInfo().title.isEmpty() ? tr(" ") : user->getInfo().title);
    ui->companyLabel->setText(user->getInfo().company.isEmpty() ? tr(" ") : user->getInfo().company);
    setState(Request);

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), SLOT(stepUp()));
    t->setInterval(60000);
    t->start();

    connect(user, SIGNAL(channelOpened()), SLOT(channelOpened()));
    connect(user, SIGNAL(channelClosed()), SLOT(channelClosed()));
    connect(this, SIGNAL(channelAction(ChannelAction)),
            user, SLOT(channelAction(ChannelAction)));
    connectSpeaker();
}

SpeakerWidget::~SpeakerWidget()
{
    delete ui;
    if (state == Stream)
        emit speakerDelete(user);
}

void SpeakerWidget::setState(State s)
{
    switch (s) {
    case Request:
        ui->volumeFrame->hide();
        ui->titleLabel->hide();
        ui->companyLabel->hide();
        ui->avatar->hide();
        ui->acceptButton->show();
        ui->delayBox->show();
        setMaximumHeight(50);
        break;
    case Stream:
        ui->volumeFrame->show();
        ui->titleLabel->show();
        ui->companyLabel->show();
        ui->avatar->show();
        ui->acceptButton->hide();
        ui->delayBox->hide();
        setMaximumHeight(130);
        emit speakerNew(user);
        break;
    }
    state = s;
}

void SpeakerWidget::on_acceptButton_clicked()
{
    emit channelAction(ChOpen);
}

void SpeakerWidget::on_dismissButton_clicked()
{
    switch (state) {
    case Request:
        emit channelAction(ChDeny);
        break;
    case Stream:
        emit channelAction(ChClose);
        break;
    }
}

void SpeakerWidget::setAmplitude(User *user, ushort amp)
{
    if (user == this->user)
        ui->volumeBar->setValue(amp);
}

void SpeakerWidget::on_volumeSlider_sliderMoved(int position)
{
    emit volumeChanged(user, position / 100.0);
}

void SpeakerWidget::stepUp()
{
    static const char *stepLabel = "%1 min";
    stepVal += 1;
    ui->delayBox->setText(tr(stepLabel).arg(stepVal));
}

void SpeakerWidget::connectSpeaker()
{
    Speaker *s = Speaker::instance();
    connect(this, SIGNAL(volumeChanged(User*,qreal)),
            s,    SLOT(setVolume(User*,qreal)));
    connect(s,    SIGNAL(audioAmpUpdated(User*,ushort)),
                  SLOT(setAmplitude(User*,ushort)));
    connect(this, SIGNAL(speakerNew(User*)),
            s,    SLOT(speakerNew(User*)));
    connect(this, SIGNAL(speakerDelete(User*)),
            s,    SLOT(speakerDelete(User*)));
}
