#include "recorder.h"

#include <QCoreApplication>
#include <QFile>

Recorder::Recorder(const UserMap &users, QObject *parent)
    : QObject(parent),
      users(users),
      recordDir(QCoreApplication::applicationDirPath()),
      recordStarted(false)
{
    moveToThread(&myThread);
    myThread.start(QThread::LowestPriority);
}

Recorder::~Recorder()
{
    myThread.terminate();
    myThread.wait();
}

void Recorder::setRecordDirectory(QString d)
{
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

void Recorder::record(QString speaker, QByteArray sample)
{
    Q_ASSERT(users.contains(speaker));
    if (!recordStarted)
        return;

    const QString filename = recordDir + "/"
            + users[speaker].name + "-" + users[speaker].company +".raw";
    QFile f(filename);
    f.open(QIODevice::Append);
    f.write(sample);
}
