#ifndef CHANNEL_WIDGET_H
#define CHANNEL_WIDGET_H

#include <QWidget>
#include <user_information.h>
#include <receiver.h>

namespace Ui {
class ChannelWidget;
}

class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(QString clientAddress,
                           UserInformation info,
                           QWidget *parent = 0);
    ~ChannelWidget();

signals:
    void volumeChanged(QString address, qreal volume);
    void closeChannelClicked(QString address);

public slots:
    void setAmplitude(QString address, ushort amp);

private slots:
    void changeVolume(int volume)
    {
        emit volumeChanged(address, volume / 100.0);
    }

    void on_closeButton_clicked();

private:
    Ui::ChannelWidget *ui;
    Receiver *receiver;
    QString address;
};

#endif // CHANNEL_WIDGET_H
