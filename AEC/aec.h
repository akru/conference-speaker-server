#ifndef AEC_H
#define AEC_H

#include <QBuffer>
#include <QByteArray>

class AEC : public QObject
{
    Q_OBJECT
public:
    AEC(QObject *parent = 0);
    QByteArray process(QByteArray &data);

public slots:
    void sync();

private:
    enum Mode {
        SYNC,
        WORK
    };
    Mode        mode;
    QBuffer     buffer;
    qint64      readPos;

    bool samplecmp(QByteArray &data);
    inline double quad(double num) { return num*num; }
    QByteArray arraySub(QByteArray &arr1, QByteArray &arr2);

    static const double TRASHOLD = 1000;
};

#endif // AEC_H
