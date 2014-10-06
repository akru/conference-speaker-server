#include "filter.h"

void Filter::enable(bool e)
{
    enabled = e;
}

void Filter::process(float sample[])
{
    if (enabled)
        processFilter(sample);
}
