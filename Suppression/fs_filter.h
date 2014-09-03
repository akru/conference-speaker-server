#ifndef FS_FILTER_H
#define FS_FILTER_H

#include "../Speaker/filter.h"
#include <QByteArray>

static const int TONE_SHIFT_TIME = 800; // ms; WARN: it divided by frame length (32 ms)

class FSFilter : public Filter
{
public:
    FSFilter();
    ~FSFilter();

    QByteArray process(const QByteArray &sample);
    QString name() { return "Frequency shifter filter."; }

private:
    int iteration;
    bool currentState;
};

#endif // FS_FILTER_H
