#ifndef SPEAKER_H
#define SPEAKER_H

#include <QList>
#include <QObject>
//#include <QThread>
#include <QTimer>
#include <filter.h>
#include <ampanalyze_filter.h>
#include "accbuffer.hpp"

class QAudioOutput;
class QAudioFormat;
class QIODevice;
typedef struct soxr * soxr_t;

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
    void incomingData(QByteArray packet);
    void reloadFilterSettings();

private slots:
    void speakHeartbeat();
    void play(const QByteArray &sample);

private:
    static const int formatSampleRate = 44100;

    bool            disabled;
    QAudioFormat    *format;
    QAudioOutput    *audio;
    QIODevice       *audio_buffer;

    AmpAnalyzeFilter *amp;
    QList<Filter *>   filters;
    AccBuffer<qint16> accBuf;

//    QThread         myThread;
    QTimer          heartbeat;
    // Improved resampler using SoX
    soxr_t          resampler;
};

#endif // SPEAKER_H
