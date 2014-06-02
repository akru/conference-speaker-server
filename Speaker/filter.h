#ifndef FILTER_H
#define FILTER_H

#include <QByteArray>

// Audio filter interface
class Filter
{
public:
    virtual ~Filter();
    virtual QByteArray process(QByteArray &data) = 0;
    virtual QString name() = 0;
};

#endif // FILTER_H
