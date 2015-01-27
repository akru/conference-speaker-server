#ifndef RECORDER_H
#define RECORDER_H

#include "gate.h"
#include <QThread>
#include <QDebug>

class Recorder : public QObject
{
    Q_OBJECT
public:
    Recorder(QObject *parent = 0);
    ~Recorder();

public slots:
    void start() {
        qDebug() << "Record started";
        recordStarted = true;
    }
    void stop() {
        qDebug() << "Record stoped";
        recordStarted = false;
    }
    void record(User *speaker, QByteArray sample);
    void record(QByteArray sample);
    void setRecordDirectory(QString d);

private:
    QString recordDir;
    bool recordStarted;
    QThread myThread;
};

#endif // RECORDER_H
