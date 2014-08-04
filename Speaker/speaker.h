#ifndef SPEAKER_H
#define SPEAKER_H

#include <QList>
#include <QObject>
#include "filter.h"
#include "debug_dialog.h"
#include <hs_filter.h>

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
#ifdef QT_DEBUG
    void setTrashHolds(qreal PAPR, qreal PHPR, qreal PNPR, qreal IMSD);
    void showDebug();
#endif
    void play(const QByteArray &packet);

private:
    void ampAnalyze(const QByteArray &sample);

    bool            disabled;
    QAudioFormat    *format;
    QAudioOutput    *audio;
    QIODevice       *audio_buffer;
    QList<Filter *> filters;
#ifdef QT_DEBUG
    HsHandle *hs;
    DebugDialog     debug_dialog;
#endif
};

#endif // SPEAKER_H
