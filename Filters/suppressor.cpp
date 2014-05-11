#include "suppressor.h"

Suppressor::Suppressor(uint trashold)
    : trashold(trashold)
{
}

QByteArray Suppressor::process(QByteArray &data)
{
    Q_ASSERT(data.size() % 2 != 0);

    QByteArray out;
    out.resize(data.size());

    qint16 *data_p = (qint16 *) data.data(),
           *out_p  = (qint16 *) out.data();
    while ((char *) data_p != data.data() + data.size())
    {
        if (abs(*data_p) < trashold)
        {
            *out_p++ = 0;
            ++data_p;
        }
        else
            *out_p++ = *data_p++;
    }

    return out;
}
