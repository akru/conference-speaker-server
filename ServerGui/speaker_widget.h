#ifndef SPEAKER_WIDGET_H
#define SPEAKER_WIDGET_H

#include <user.h>
#include <QWidget>

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

    explicit SpeakerWidget(User *user,
                           QWidget *parent = 0);
    ~SpeakerWidget();

    State getState() const
    { return state; }

signals:
    void dismiss();
    void volumeChanged(User*, qreal);
    void speakerNew(User*);
    void speakerDelete(User*);

public slots:
    void setState(State s);
    void setAmplitude(User *user, ushort amp);
    void on_dismissButton_clicked();

private slots:
    void on_acceptButton_clicked();
    void on_volumeSlider_sliderMoved(int position);
    void stepUp();
    void connectSpeaker();

    void channelOpened()
    { setState(Stream); }
    void channelClosed()
    { emit dismiss(); }

private:
    Ui::SpeakerWidget *ui;
    User              *user;
    State              state;
    uint               stepVal;
};

#endif // SPEAKER_WIDGET_H
