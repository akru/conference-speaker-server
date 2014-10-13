#include "gate_filter.h"
#include <cmath>

GateFilter::GateFilter(float raiseTHdB,
                       float fallTHdB,
                       float attackTime,
                       float holdTime,
                       float releaseTime)
    : raiseTHdB(raiseTHdB),
      raiseTH(pow(10.0, raiseTHdB/20.0)),
      fallTHdB(fallTHdB),
      fallTH(pow(10.0, fallTHdB/20.0)),
      attackTime(attackTime),
      holdTime(holdTime),
      releaseTime(releaseTime),
      state(Closed),
      scaleVal(0),
      scaleUpStep(1.0 / attackTime / sample_rate),
      scaleDownStep(1.0 / releaseTime / sample_rate),
      holdCtr(0)
{
}

GateFilter::~GateFilter()
{
}

void GateFilter::processFilter(float sample[])
{
    for (short i = 0; i < sample_length; ++i)
        sample[i] = gateLogic(sample[i]);
}

float GateFilter::gateLogic(float s)
{    
    switch (state) {
    case Closed:
        if (fabs(s) > raiseTH)
            state = Opened;
        break;

    case Opened:
        if (fabs(s) < fallTH)
        {
            holdCtr = 0;
            state = Holding;
            break;
        }
        // Scale up
        if (scaleVal < 1)
            scaleVal += scaleUpStep;
        break;

    case Holding:
        if (fabs(s) > raiseTH)
            state = Opened;
        else
            if (holdCtr++ > holdTime * sample_rate)
                state = Releasing;
        break;

    case Releasing:
        // Scale down
        if (scaleVal > 0)
            scaleVal -= scaleDownStep;
        else
            state = Closed;
        break;
    }
    return s * scaleVal;
}
