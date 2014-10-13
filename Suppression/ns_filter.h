#ifndef NS_FILTER_H
#define NS_FILTER_H

#include "filter.h"
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

    NSFilter();
    ~NSFilter();

    void setPolicy(Level level)
    {
        WebRtcNs_set_policy(ns_ptr, level);
    }

    void processFilter(float sample[]);
    QString name() { return "Noise suppression"; }
    void reloadSettings();

private:
    NsHandle *ns_ptr;
};

#endif // NS_FILTER_H
