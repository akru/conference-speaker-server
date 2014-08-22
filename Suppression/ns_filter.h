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

    NSFilter(Level level, int count, int trashold);
    ~NSFilter();

    QByteArray process(const QByteArray &sample);
    QString name() { return "Noise suppression"; }

protected:
    QByteArray postSuppression(const QByteArray &sample);

private:
    NsHandle *ns_ptr;
    int count, trashold;
};

#endif // NS_FILTER_H
