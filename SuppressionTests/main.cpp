#include <hs_filter.h>
#include <bandswitch_filter.h>
#include <highpass_filter.h>
#include <equalizer_filter.h>
#include <QDebug>
#include <QFile>
#include <QTime>
#include <windows_private.h>

#include <cmath>

int main()
{
//    HighPassFilter hpf;
//    HSFilter hsf;
    //BandswitchFilter bsf;
    float eqs[Filter::sample_length];
    for (short i = 0; i<Filter::sample_length; ++i) {
        eqs[i] = sin(i / 4 / PI_F) < 0 ? 0 : sin(i / 4 / PI_F);
        qDebug() << "H[" << i << "]=" <<eqs[i];
    }
    EqualizerFilter eq(0.01, eqs, kBlackmanWindow256);

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

            // Normalization
            qint16 *rawp = (qint16 *) sam.data();
            float sample[Filter::sample_length];
            float *fltp = sample;
            while ((char *) rawp < sam.data() + sam.length()) {
//                qDebug() << "I:" << *rawp;
                *fltp++ = (float) *rawp++;
            }

//            hsf.process(sample);
//            hpf.process(sample);
            eq.process(sample);


            rawp = (qint16 *) sam.data();
            fltp = sample;
            while ((char *) rawp < sam.data() + sam.length()) {
//                qDebug() << "O:" << *fltp;
                *rawp++ = (qint16) *fltp++;
            }

            phones.write(sam);
            qDebug() << t.elapsed() << "passed" << tm++ * 32 << "ms";
        }
    }
    return 0;
}
