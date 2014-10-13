#include "gate_filter.h"
#include <cmath>
#include <QSettings>
#include <QDebug>

GateFilter::GateFilter()
    : state(Closed),
      scaleVal(0),
      holdCtr(0)
{
}

GateFilter::~GateFilter()
{
}

void GateFilter::setParams(float raiseTH,
                           float fallTH,
                           float attackTime,
                           float holdTime,
                           float releaseTime)
{
    this->raiseTH = pow(10.0, raiseTH/20.0);
    this->fallTH = pow(10.0, fallTH/20.0);
    this->attackTime = attackTime;
    this->holdTime = holdTime;
    this->releaseTime = releaseTime;
    scaleUpStep = 1.0 / attackTime / sample_rate;
    scaleDownStep = 1.0 / releaseTime / sample_rate;
}

void GateFilter::reloadSettings()
{
    QSettings s(settingsFiltename(), QSettings::IniFormat);
    enable(s.value("gate-enable", true).toBool());
    setParams(s.value("gate-raise", 0.05).toFloat(),
              s.value("gate-fall", 0.2).toFloat(),
              s.value("gate-attack", 0.1).toFloat(),
              s.value("gate-hold", 0.2).toFloat(),
              s.value("gate-release", 0.1).toFloat());
    qDebug() << "Gate settings reloaded";
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
