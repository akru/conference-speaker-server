#ifndef HS_FILTER_H
#define HS_FILTER_H

#include "../Speaker/filter.h"
#include "../Speaker/sample.h"
#include "howling_suppression.h"

class HSFilter : public Filter
{
public:
    HSFilter(quint32 sample_rate);
    ~HSFilter();

    Sample process(const Sample &sample);
    QString name() { return "Howling suppression"; }

private:
    HsHandle *hs_ptr;
};

#endif // HS_FILTER_H
