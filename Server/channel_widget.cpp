#include "channel_widget.h"
#include "ui_channel_widget.h"

ChannelWidget::ChannelWidget(UserInformation info,
                             Receiver *channel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelWidget),
    receiver(channel)
{
    ui->setupUi(this);
    ui->label->setText(info.name + channel->getPeerAddress().toString());

    connect(ui->volumeDial, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
    connect(ui->volumeDial, SIGNAL(valueChanged(int)),
            ui->volume, SLOT(display(int)));
    connect(this, SIGNAL(volumeChanged(qreal)), channel, SLOT(setVolume(qreal)));

    ui->volumeDial->setValue(50);
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::on_closeButton_clicked()
{
    emit closeChannelClicked(receiver->getPeerAddress().toString());
    close();
}
