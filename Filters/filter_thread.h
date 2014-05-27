#ifndef FILTER_THREAD_H
#define FILTER_THREAD_H

#include "filter.h"
#include <QThread>

class FIlterThread : public QObject
{
    Q_OBJECT
public:
    FIlterThread();

signals:
    void ready(QByteArray data);

public slots:
    void process(QByteArray data);
    void append(Filter *f);

private:
    QList<Filter *> filters;
    QThread *me;
};

#endif // FILTER_THREAD_H
