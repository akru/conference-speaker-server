#include "recorder.h"
#include "speaker.h"

#include <QCoreApplication>
#include <QFile>
#include <QDebug>

Recorder::Recorder(QObject *parent)
    : QObject(parent),
      recordDir(QCoreApplication::applicationDirPath()),
      recordStarted(false)
{
    moveToThread(&myThread);
    myThread.start(QThread::LowestPriority);

    Speaker *s = Speaker::instance();
    connect(s, SIGNAL(sampleReady(QByteArray)), SLOT(record(QByteArray)));
    connect(s, SIGNAL(sampleReady(User*,QByteArray)),
               SLOT(record(User*,QByteArray)));
}

Recorder::~Recorder()
{
    myThread.terminate();
    myThread.wait();
}

void Recorder::setRecordDirectory(QString d)
{
    qDebug() << "Storage path changed to" << d;
    recordDir = d;
}

void Recorder::record(QByteArray sample)
{
    if (!recordStarted)
        return;

    QFile f(recordDir + "/all.raw");
    f.open(QIODevice::Append);
    f.write(sample);
}

void Recorder::record(User *speaker, QByteArray sample)
{
    if (!recordStarted)
        return;

    const QString filename = recordDir + "/"
            + speaker->getInfo().name + "-" + speaker->getInfo().company +".raw";
    QFile f(filename);
    f.open(QIODevice::Append);
    f.write(sample);
}
