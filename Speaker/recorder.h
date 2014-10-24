#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QByteArray>

class Recorder : public QObject
{
    Q_OBJECT
public:
    Recorder(QObject *parent = 0);

public slots:
    void record(QString speaker, QByteArray sample);
    void record(QByteArray sample);
    void setRecordDirectory(QString d);

private:
    QString recordDir;
};

#endif // RECORDER_H
