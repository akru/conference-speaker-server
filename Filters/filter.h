#ifndef FILTER_H
#define FILTER_H

#include <QByteArray>

class Filter
{
public:
    virtual QByteArray process(QByteArray &data) = 0;
};

#endif // FILTER_H
