#ifndef SPEAKER_H
#define SPEAKER_H

#include <QMap>
#include <QTimer>
#include <QThread>

class QAudioOutput;
class Processing;
class QIODevice;
class User;

typedef struct soxr * soxr_t;
typedef QMap<User*, Processing*> ProcMap;

class Speaker : public QObject
{
    Q_OBJECT
    Speaker(QObject *parent = 0);

public:
    // Speaker singletone instance
    static Speaker * instance();

    // QAudioFormat sample rate for output samples
    static const uint formatSampleRate = 44100;

    inline bool isDisabled() const { return disabled; }

signals:
    // Current stream amplitude in percent [0..100]
    void audioAmpUpdated(User*, ushort);
    // Recording cases:
    // Unmixed samples
    void sampleReady(User*, QByteArray);
    // Mixed samples
    void sampleReady(QByteArray);

public slots:
    // Set global volume [0..1]
    void setVolume(qreal volume);
    // Set speaker volume [0..1]
    void setVolume(User* speaker, qreal volume);
    void incomingData(User* speaker, QByteArray packet);
    void reloadFilterSettings();
    void speakerNew(User* id);
    void speakerDelete(User* id);

private slots:
    void speakHeartbeat();
    void play(QByteArray sample);

private:
    bool          disabled;
    QAudioOutput *audio;
    QIODevice    *audio_buffer;
    // Improved resampler using SoX
    soxr_t        resampler;
    // Speaker filters
    ProcMap       proc;
    // Separate time-critical speaker thread
    QThread       myThread;
    QTimer        heartbeat;
};

#endif // SPEAKER_H
