#ifndef SPEAKER_H
#define SPEAKER_H

#include <QList>
#include <QObject>
#include <QThread>
#include <QTimer>
#include "filter.h"
#include "accbuffer.hpp"

#ifdef QT_DEBUG
#include "debug_dialog.h"
#include <hs_filter.h>
#endif

class QAudioOutput;
class QAudioFormat;
class QIODevice;

static const quint16 norm_int16 = (1<<15);

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
#ifdef QT_DEBUG
    void setTrashHolds(qreal PAPR, qreal PHPR, qreal PNPR, qreal IMSD);
    void showDebug();
#endif
    void play(QByteArray packet);

private slots:
    void ampAnalyze(const QByteArray &sample);
    void speakHeartbeat();

private:
    bool            disabled;
    QAudioFormat    *format;
    QAudioOutput    *audio;
    QIODevice       *audio_buffer;

    QList<Filter *> filters;
    AccBuffer<qint16> accBuf;

    QThread         myThread;
    QTimer          heartbeat;
#ifdef QT_DEBUG
    DebugDialog     *debug_dialog;
#endif
};

#endif // SPEAKER_H
