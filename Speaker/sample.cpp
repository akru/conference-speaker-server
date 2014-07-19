#include "sample.h"
#include <QDataStream>
#include <QDebug>

SampleMeta::SampleMeta(const QByteArray &packet)
    : bad(false)
{
    QDataStream ds(packet);
    ds >> version >> size;
    if (version != protoVersion)
    {
        qDebug() << "Bad packet version" << version;
        bad = true;
        return;
    }
    ds >> id >> timestamp;
    qDebug() << "Constructed Sample(ver ="
             << version << " size=" << size
             << " id=" << id << " ts=" << timestamp << ")";
}

Sample::Sample(const QByteArray &rawPacket) :
    _meta(SampleMeta(rawPacket))
{
    rawSample = rawPacket;
    rawSample.remove(0, SampleMeta::headerSize);
}

Sample::Sample(const SampleMeta &extMeta, const QByteArray &extSample) :
    _meta(extMeta),
    rawSample(extSample)
{
}

SampleMeta Sample::meta() const
{
    return _meta;
}

const qint16 * Sample::data() const
{
    return (qint16 *) rawSample.data();
}

int Sample::length() const
{
    return rawSample.length();
}
