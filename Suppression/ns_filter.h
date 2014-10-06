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

    NSFilter(Level level);
    ~NSFilter();

    void processFilter(float sample[]);
    QString name() { return "Noise suppression"; }

private:
    NsHandle *ns_ptr;
};

#endif // NS_FILTER_H
