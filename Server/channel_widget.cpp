#include "channel_widget.h"
#include "ui_channel_widget.h"

ChannelWidget::ChannelWidget(QString clientAddress,
                             UserInformation info,
                             Receiver *channel,
                             QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ChannelWidget),
      receiver(channel),
      address(clientAddress)
{
    ui->setupUi(this);
    ui->nameLabel->setText(info.name);
    ui->companyLabel->setText(info.company);

    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
    connect(this, SIGNAL(volumeChanged(qreal)), channel, SLOT(setVolume(qreal)));
    connect(channel, SIGNAL(audioAmpUpdated(int)), ui->volumeBar, SLOT(setValue(int)));

    ui->volumeSlider->setValue(50);
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::on_closeButton_clicked()
{
    emit closeChannelClicked(address);
}
