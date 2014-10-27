#ifndef RECORDER_H
#define RECORDER_H

#include "server.h"
#include <QThread>
#include <QDebug>

class Recorder : public QObject
{
    Q_OBJECT
public:
    Recorder(const UserMap &users, QObject *parent = 0);

public slots:
    void start() {
        qDebug() << "Record started";
        recordStarted = true;
    }
    void stop() {
        qDebug() << "Record stoped";
        recordStarted = false;
    }
    void record(QString speaker, QByteArray sample);
    void record(QByteArray sample);
    void setRecordDirectory(QString d);

private:
    const UserMap &users;
    QString recordDir;
    bool recordStarted;
    QThread myThread;
};

#endif // RECORDER_H
