#ifndef SPEAKER_H
#define SPEAKER_H

#include <QMap>
#include <QTimer>
#include <QObject>
#include <QThread>

typedef struct soxr * soxr_t;

class QAudioOutput;
class Processing;
class QIODevice;

class Speaker : public QObject
{
    Q_OBJECT
public:
    explicit Speaker(QObject *parent = 0);
    ~Speaker();

    // QAudioFormat sample rate for output samples
    static const uint formatSampleRate = 44100;

signals:
    void audioAmpUpdated(QString speaker, ushort amplitude);
    void sampleReady(QString speaker, QByteArray sample);
    void sampleReady(QByteArray sample);

public slots:
    void setVolume(qreal volume);
    void setVolume(QString speaker, qreal volume);
    void incomingData(QString speaker, QByteArray packet);
    void reloadFilterSettings();
    void speakerNew(QString id);
    void speakerDelete(QString id);

private slots:
    void speakHeartbeat();
    void play(QByteArray sample);

private:
    QAudioOutput               *audio;
    QIODevice                  *audio_buffer;
    // Improved resampler using SoX
    soxr_t                      resampler;
    // Speaker filters
    QMap<QString, Processing *> proc;
    // Separate time-critical speaker thread
    QThread                     myThread;
    QTimer                      heartbeat;
};

#endif // SPEAKER_H
