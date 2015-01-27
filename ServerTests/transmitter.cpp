#include "transmitter.h"
#include <QTimer>

Transmitter::Transmitter(ChannelInformation channel,
                         QObject *parent)
    : QObject(parent), ch(channel)
{
    qDebug() << "Connecting to:" << channel.toJson();
    connect(&sock, SIGNAL(connected()), this, SLOT(connected()));
    sock.connectToHost(channel.host, channel.port);
}

void Transmitter::connected()
{
    qDebug() << "Connected!";
    QAudioFormat desiredFormat;
    desiredFormat.setSampleSize(16);
    desiredFormat.setChannelCount(1);
    desiredFormat.setSampleRate(22050);
    desiredFormat.setCodec("audio/pcm");
    desiredFormat.setSampleType(QAudioFormat::SignedInt);
    desiredFormat.setByteOrder(QAudioFormat::LittleEndian);

    decoder = new QAudioDecoder(this);
    decoder->setAudioFormat(desiredFormat);
    decoder->setSourceFilename("/home/khassanov/test.wav");

    connect(decoder, SIGNAL(bufferReady()), SLOT(readBuffer()));
    decoder->start();
    if (!decoder->bufferAvailable())
    {
        qDebug() << decoder->error();
        qDebug() << decoder->errorString();
    }
    QTimer *t = new QTimer(this);
    t->setInterval(40);
    connect(t, SIGNAL(timeout()), SLOT(sendData()));
    t->start();
}

void Transmitter::readBuffer()
{
    QAudioBuffer buffer = decoder->read();
    qDebug() << "Transmitter: decoded" << buffer.byteCount() << "bytes";
    data.push_back(QByteArray((const char *)buffer.constData(), buffer.byteCount()));
}

void Transmitter::sendData()
{
        if (data.length())
        {
            sock.writeDatagram(data.first(), QHostAddress(ch.host), ch.port);
            data.pop_front();
        }
}
