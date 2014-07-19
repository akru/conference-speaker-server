#include <hs_filter.h>
#include <QDebug>
#include <QFile>
#include <QTime>

int main()
{
    HSFilter f(8000);

    QFile audio("in.wav");
    audio.open(QIODevice::ReadOnly);

    QFile phones("out.wav");
    phones.open(QIODevice::WriteOnly);

    SampleMeta m;
    QTime t;
    while (!audio.atEnd())
    {
        Sample sam(m, audio.read(160));
        qDebug() << sam.length();
        t = QTime::currentTime();
        Sample res = f.process(sam);
        phones.write((char *) res.data(), res.length());
        qDebug() << t.elapsed();
    }

    audio.close();
    phones.close();

    return 0;
}
