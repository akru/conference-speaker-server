#ifndef GATE_FILTER_H
#define GATE_FILTER_H

#include "../Speaker/filter.h"

class GateFilter : public Filter
{
public:
    GateFilter();
    ~GateFilter();

    void process(float sample[]);
    QString name() { return "Gate"; }
private:
    float attack_time;
    float lower_TH;
    float upper_TH;
    float hold_time;
};

#endif // GATE_FILTER_H
