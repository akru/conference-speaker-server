#ifndef SAMPLE_H
#define SAMPLE_H

#include <QByteArray>

struct SampleMeta {
    bool    bad;
    // Header information
    quint8 version;
    qint16 size;
    // Sample information
    qint32 id;
    qint64 timestamp;
    // Constructor
    SampleMeta() {}
    SampleMeta(const QByteArray &packet);
    // Constants
    static const quint8 protoVersion = 1;
    static const quint8 headerSize   =
            sizeof(quint8) + sizeof(qint16) +
            sizeof(qint32) + sizeof(qint64);
};

class Sample
{
public:
    Sample(const QByteArray &rawPacket);
    Sample(const SampleMeta &extMeta, const QByteArray &extSample);
    // Sample meta struct
    SampleMeta meta() const;
    // Source data
    const qint16 *data() const;
    // Length in bytes
    int length() const;
private:
    SampleMeta _meta;
    QByteArray rawSample;
};

#endif // SAMPLE_H
