#include "lms_filter.h"
#include "lms.h"
#include <QDebug>

LMSFilter::LMSFilter(uint length, double rho, double gamma)
    : rho(rho), gamma(gamma)
{
    ek = QByteArray(length, 0);
    xk = QByteArray(length, 0);
    wn.resize(length);
}

QByteArray LMSFilter::process(QByteArray &data)
{
    qint16 *pd = (qint16 *) data.data();
    qint16 *px = (qint16 *) xk.data();
    qint16 *pe = (qint16 *) ek.data();
    while ((char *) pd != data.data() + data.length())
    {
        qint16 yk = splab::lmsNormalize((double) *px++, (double) *pd, wn, rho, gamma);
        *pe++ = *pd++ - yk;
    }
    dump_stats();
    xk = ek;
    return ek;
}

void LMSFilter::dump_stats()
{
    qDebug() << "ek:" << ek.toHex();
    qDebug() << "xk:" << xk.toHex();
    QString wnstr = "wn:";
    foreach (double w, wn) {
        wnstr += QString(" ") + QString::number(w);
    }
    qDebug() << wnstr;
}
