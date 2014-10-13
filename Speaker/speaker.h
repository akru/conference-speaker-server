#ifndef SPEAKER_H
#define SPEAKER_H

#include <QList>
#include <QObject>
//#include <QThread>
#include <QTimer>
#include "../Suppression/filter.h"
#include "accbuffer.hpp"

class QAudioOutput;
class QAudioFormat;
class QIODevice;

static const quint16 norm_int16 = 32768;

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
    void play(QByteArray packet);
    void reloadFilterSettings();

private slots:
    void ampAnalyze(const float sample[]);
    void speakHeartbeat();

private:
    bool            disabled;
    QAudioFormat    *format;
    QAudioOutput    *audio;
    QIODevice       *audio_buffer;

    QList<Filter *> filters;
    AccBuffer<qint16> accBuf;

//    QThread         myThread;
    QTimer          heartbeat;
};

#endif // SPEAKER_H
