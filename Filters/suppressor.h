#ifndef SUPPRESSOR_H
#define SUPPRESSOR_H

#include "filter.h"

class Suppressor : public Filter
{
public:
    Suppressor(uint trashold = 10);

    QByteArray process(QByteArray &data);

private:
    uint trashold;
};

#endif // SUPPRESSOR_H
