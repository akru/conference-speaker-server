#include "channel_widget.h"
#include "ui_channel_widget.h"

ChannelWidget::ChannelWidget(QString clientAddress,
                             UserInformation info,
                             QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ChannelWidget),
      address(clientAddress)
{
    ui->setupUi(this);
    ui->nameLabel->setText(info.name);
    ui->companyLabel->setText(info.company);

    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));

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

void ChannelWidget::setAmplitude(QString address, ushort amp)
{
    if (address == this->address)
        ui->volumeBar->setValue(amp);
}
