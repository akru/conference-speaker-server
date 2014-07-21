#include <hs_filter.h>
#include <biquad.h>
#include <QDebug>
#include <QFile>
#include <QTime>

#define HOWLING_SUPPRESSION

int main()
{
#ifdef HOWLING_SUPPRESSION
    HSFilter f(8000);
#else
    HsBiquadParams f;
    Hs_BiquadInit(&f);
    f.freq = 600;
    f.peakGain = -10;
    Hs_BiquadCalc(&f);
#endif

    QFile audio("in.wav");
    audio.open(QIODevice::ReadOnly);

    QFile phones("out.wav");
    phones.open(QIODevice::WriteOnly);

    QTime t;
    QByteArray sam, res;
    res.resize(512);
    while (!audio.atEnd())
    {
        sam = audio.read(512);
        qDebug() << "len" << sam.length();
        if (sam.length() == 512)
        {
            t = QTime::currentTime();
#ifdef HOWLING_SUPPRESSION
            res = f.process(sam);
#else
            qint16 *output = (qint16 *) res.data();
            const qint16 *input_p = (qint16 *) sam.data();
            while ((char *)input_p < sam.data() + 512)
                *output++ = Hs_BiquadProcess(&f, *input_p++);
#endif
            phones.write(res);
            qDebug() << t.elapsed();
        }
    }

    audio.close();
    phones.close();

    return 0;
}
