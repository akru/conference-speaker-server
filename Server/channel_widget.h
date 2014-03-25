#ifndef CHANNEL_WIDGET_H
#define CHANNEL_WIDGET_H

#include <QWidget>
#include <user_information.h>
#include "receiver.h"

namespace Ui {
class ChannelWidget;
}

class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(QString clientAddress, UserInformation info,
                           Receiver *channel, QWidget *parent = 0);
    ~ChannelWidget();

signals:
    void volumeChanged(qreal volume);
    void closeChannelClicked(QString address);

private slots:
    void changeVolume(int volume)
    {
        emit volumeChanged(volume / 100.0);
    }

    void on_closeButton_clicked();

private:
    Ui::ChannelWidget *ui;
    Receiver *receiver;
    QString address;
};

#endif // CHANNEL_WIDGET_H
