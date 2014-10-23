#ifndef AMPANALYZE_FILTER_H
#define AMPANALYZE_FILTER_H

#include "filter.h"

class AmpAnalyzeFilter : public Filter
{
public:
    AmpAnalyzeFilter() : amp(0) {}
    ~AmpAnalyzeFilter() {}

    void processFilter(float sample[]);
    QString name() { return "Amplitude analyzer"; }
    void reloadSettings() {}

    inline int getAmp() { return amp; }

private:
    int amp;
};

#endif // AMPANALYZE_FILTER_H
