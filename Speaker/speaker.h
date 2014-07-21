#ifndef SPEAKER_H
#define SPEAKER_H

#include <QList>
#include <QObject>
#include "filter.h"

class QAudioOutput;
class QAudioFormat;
class QIODevice;

class Speaker : public QObject
{
    Q_OBJECT
public:
    explicit Speaker(QObject *parent = 0);
    ~Speaker();

    inline bool isDisabled()
    {
        return disabled;
    }

signals:
    void audioAmpUpdated(int amplitude);

public slots:
    void setVolume(qreal volume);
    void play(const QByteArray &packet);

private slots:

private:
    void ampAnalyze(const QByteArray &sample);

    bool            disabled;
    QAudioFormat    *format;
    QAudioOutput    *audio;
    QIODevice       *audio_buffer;
    QList<Filter *> filters;
};

#endif // SPEAKER_H
