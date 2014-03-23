#include "transmitter.h"

Transmitter::Transmitter(ChannelInformation channel,
                         QObject *parent)
    : QObject(parent)
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
    desiredFormat.setSampleRate(8000);
    desiredFormat.setCodec("audio/pcm");
    desiredFormat.setSampleType(QAudioFormat::SignedInt);
    desiredFormat.setByteOrder(QAudioFormat::LittleEndian);

    decoder = new QAudioDecoder(this);
    decoder->setAudioFormat(desiredFormat);
    decoder->setSourceFilename("/home/akru/test.wav");

    connect(decoder, SIGNAL(bufferReady()), SLOT(readBuffer()));
    decoder->start();
    if (!decoder->bufferAvailable())
    {
        qDebug() << decoder->error();
        qDebug() << decoder->errorString();
    }
}

void Transmitter::readBuffer()
{
    QAudioBuffer buffer = decoder->read();
    qDebug() << "Transmitter: decoded" << buffer.byteCount() << "bytes";
    sock.write((const char *)buffer.constData(), buffer.byteCount());
}
