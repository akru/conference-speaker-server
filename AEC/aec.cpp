#include "aec.h"
#include <QDebug>

AEC::AEC(QObject *parent)
    : QObject(parent)
{
    mode = SYNC;
}

void AEC::sync()
{
    mode = SYNC;
}

QByteArray AEC::process(QByteArray &data)
{
    QByteArray echo, result = data;

    buffer.seek(buffer.size());
    buffer.write(data);
    switch (mode) {
    case SYNC:
        if (samplecmp(data))
            mode = WORK;
        else
            break;
    case WORK:
        if (buffer.size() - readPos >= data.size())
        {
            buffer.seek(readPos);
            readPos += data.size();
            echo = buffer.read(data.size());
            result = arraySub(data, echo);
        }
        else
        {
            qDebug() << "Buffer empty > SYNC";
            mode = SYNC;
        }
        break;
    }
    return result;
}

/*
 * Sample comparation method.
 * The assumption:
 *  - sample time >> packet time;
 * This method calcs estimated quadro error of
 * sample by input buffer difference.
 * And return true when err above threshold.
 */
bool AEC::samplecmp(QByteArray &data)
{
    int buffer_size = buffer.data().size() / 2;
    if (buffer_size < data.size())
    {
        qDebug() << "samplecmp :: short buffer";
        return false;
    }

    qint16 *sample_p = (qint16 *) buffer.data().data();
    qint16 *data_p;
    double quadError = 0;

    while (sample_p != (qint16 *) buffer.data().data() + buffer_size)
    {
        data_p = (qint16 *) data.data();
        while (data_p != (qint16 *) data.data() + data.size() / 2)
            quadError += quad(*data_p++ - *sample_p);
        quadError /= data.size() / 2;
        qDebug() << "QuadError:" << quadError;
        if (quadError < TRASHOLD)
        {
            readPos = (qint64) sample_p / 2;
            return true;
        }
        ++sample_p;
    }
    return false;
}

QByteArray AEC::arraySub(QByteArray &arr1, QByteArray &arr2)
{
    Q_ASSERT(arr1.size() == arr2.size());

    QByteArray res;
    res.resize(arr1.size());

    qint16 *res_p  = (qint16 *) res.data();
    qint16 *arr1_p = (qint16 *) arr1.data();
    qint16 *arr2_p = (qint16 *) arr2.data();

    while (res_p != (qint16 *) res.data() + res.size() / 2)
        *res_p++ = *arr1_p++ - *arr2_p++;

    return res;
}
