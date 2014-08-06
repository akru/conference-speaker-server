#include <hs_filter.h>
#include <bandswitch_filter.h>
#include <highpass_filter.h>
#include <QDebug>
#include <QFile>
#include <QTime>

int main()
{
    HighPassFilter hpf;
    HSFilter hsf(8000);
    BandswitchFilter bsf(8000);

    QFile audio("in.wav");
    audio.open(QIODevice::ReadOnly);

    QFile phones("out.wav");
    phones.open(QIODevice::WriteOnly);

    QTime t;
    QByteArray sam, res;
    res.resize(512);  short tm = 0;
    while (!audio.atEnd())
    {
        sam = audio.read(512);
        if (sam.length() == 512)
        {
            t = QTime::currentTime();

            res = bsf.process(
                        hsf.process(
                            hpf.process(sam)
                            )
                        );

            phones.write(res);
            qDebug() << t.elapsed() << "passed" << tm++ * 32 << "ms";
        }
    }
    return 0;
}
