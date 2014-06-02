#ifndef SUPPRESSOR_H
#define SUPPRESSOR_H

#include "filter.h"
#include <QString>

class Suppressor : public Filter
{
public:
    Suppressor(int trashold = 10);

    QByteArray process(QByteArray &data);
    QString name() { return "Suppressor"; }

private:
    int trashold;
};

#endif // SUPPRESSOR_H
