#ifndef NS_FILTER_H
#define NS_FILTER_H

#include "../Speaker/filter.h"
//#include "../Speaker/sample.h"
#include "noise_suppression.h"

class NSFilter : public Filter
{
public:
    enum Level {
        VeryLow = 0,
        Low,
        Medium,
        High
    };

    NSFilter(quint32 sample_rate, Level level);
    ~NSFilter();

    QByteArray process(const QByteArray &sample);
    QString name() { return "Noise suppression"; }

private:
    NsHandle *ns_ptr;
    short     len10ms;
};

#endif // NS_FILTER_H
