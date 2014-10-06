#include "ns_filter.h"
#include "noise_suppression.h"

#include <cmath>
#include <QDebug>

NSFilter::NSFilter(Level level)
{
    qDebug() << "NSFilter(" << level << ")";

    if (!WebRtcNs_Create(&ns_ptr))
    {
        WebRtcNs_Init(ns_ptr, sample_rate);
        WebRtcNs_set_policy(ns_ptr, level);
    }
    else
        qWarning() << "Unable to create noise supression filter";
}

NSFilter::~NSFilter()
{
    WebRtcNs_Free(ns_ptr);
}

void NSFilter::processFilter(float sample[])
{
    float out[sample_length];

    WebRtcNs_Process(ns_ptr, sample, out);
    WebRtcNs_Process(ns_ptr, sample + Filter::sample_length / 2, out + Filter::sample_length / 2);

    for (short i = 0; i < sample_length; ++i)
        sample[i] = out[i];
}
