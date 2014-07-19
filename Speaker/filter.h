#ifndef FILTER_H
#define FILTER_H

#include "sample.h"
#include <QString>

// Audio filter interface
class Filter
{
public:
    virtual ~Filter() {}
    virtual Sample process(const Sample &sample) = 0;
    virtual QString name() = 0;
};

#endif // FILTER_H
