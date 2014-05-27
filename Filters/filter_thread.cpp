#include "filter_thread.h"

FIlterThread::FIlterThread()
    : me(new QThread)
{
    this->moveToThread(me);
    me->start();
}

void FIlterThread::process(QByteArray data)
{
    QByteArray out = data;
    foreach (Filter *f, filters) {
        out = f->process(out);
    }
    emit ready(out);
}

void FIlterThread::append(Filter *f)
{
    filters.append(f);
}
