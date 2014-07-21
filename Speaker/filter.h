#ifndef FILTER_H
#define FILTER_H

#include <QByteArray>
#include <QString>

// Audio filter interface
class Filter
{
public:
    virtual ~Filter() {}
    virtual QByteArray process(const QByteArray &sample) = 0;
    virtual QString name() = 0;
};

#endif // FILTER_H
