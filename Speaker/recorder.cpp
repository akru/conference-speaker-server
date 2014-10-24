#include "recorder.h"

#include <QCoreApplication>
#include <QFile>

Recorder::Recorder(QObject *parent)
    : QObject(parent),
      recordDir(QCoreApplication::applicationDirPath())
{
}

void Recorder::setRecordDirectory(QString d)
{
    recordDir = d;
}

void Recorder::record(QByteArray sample)
{
    QFile f(recordDir + "/all.raw");
    f.open(QIODevice::Append);
    f.write(sample);
}

void Recorder::record(QString speaker, QByteArray sample)
{
    QFile f(recordDir + "/" + speaker +".raw");
    f.open(QIODevice::Append);
    f.write(sample);
}
