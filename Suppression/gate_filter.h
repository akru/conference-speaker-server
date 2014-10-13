#ifndef GATE_FILTER_H
#define GATE_FILTER_H

#include "../Speaker/filter.h"

class GateFilter : public Filter
{
    enum GateState {
        Closed,
        Opened,
        Holding,
        Releasing
    };

public:
    GateFilter(float raiseTH,
               float fallTH,
               float attackTime,
               float holdTime,
               float releaseTime);
    ~GateFilter();

    void processFilter(float sample[]);
    QString name() { return "Gate"; }

protected:
    float raiseTH;
    float fallTH;
    float attackTime;
    float holdTime;
    float releaseTime;

private:
    GateState state;
    float scaleVal;

    float scaleUpStep;
    float scaleDownStep;
    unsigned int holdCtr;

    float gateLogic(float s);
};

#endif // GATE_FILTER_H
