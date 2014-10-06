#include "gate_filter.h"

GateFilter::GateFilter(float raiseTH,
                       float fallTH,
                       float attackTime,
                       float holdTime,
                       float releaseTime)
    : raiseTH(raiseTH),
      fallTH(fallTH),
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

void GateFilter::process(float sample[])
{
    for (short i = 0; i < sample_length; ++i)
        sample[i] = gateLogic(sample[i]);
}

float GateFilter::gateLogic(float s)
{
    switch (state) {
    case Closed:
        if (s > raiseTH)
            state = Opened;
        break;

    case Opened:
        if (s < fallTH)
        {
            holdCtr = 0;
            state = Holding;
        }
        // Scale up
        if (scaleVal < 1)
            scaleVal += scaleUpStep;
        break;

    case Holding:
        if (s > raiseTH)
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
        {
            scaleVal = 0;
            state = Closed;
        }
        break;
    }
    return s * scaleVal;
}
