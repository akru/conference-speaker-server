#ifndef SPEAKER_WIDGET_H
#define SPEAKER_WIDGET_H

#include <QWidget>
#include <user_information.h>

namespace Ui {
class SpeakerWidget;
}

class SpeakerWidget : public QWidget
{
    Q_OBJECT
public:
    enum State {
        Request,
        Stream
    };

    explicit SpeakerWidget(const QString &address,
                           const UserInformation &info,
                           QWidget *parent = 0);
    ~SpeakerWidget();

    State getState() const
    { return state; }

signals:
    void requestAccepted(QString);
    void requestDiscarded(QString);
    void closeClicked(QString);
    void volumeChanged(QString address, qreal volume);

public slots:
    void setState(State s);
    void setAmplitude(QString address, ushort amp);
    void on_dismissButton_clicked();

private slots:
    void on_acceptButton_clicked();
    void on_volumeSlider_sliderMoved(int position);
    void stepUp();

private:
    Ui::SpeakerWidget *ui;
    QString myAddress;
    State state;
    uint stepVal;
};

#endif // SPEAKER_WIDGET_H
