#include <ns_filter.h>
#include <hs_filter.h>
#include <equalizer_filter.h>
#include <pitch_shift_filter.h>

#include <QDebug>
#include <QFile>
#include <QTime>
#include <windows_private.h>

#include <cmath>




int main()
{
//    NSFilter nsf;
////    PitchShiftFilter psf(1.04, 4);

//    float eqs[Filter::sample_length];
//    for (short i = 0; i<Filter::sample_length / 2 + 1; ++i) {
//        eqs[i] = 1;//sin(i / 4 / PI_F) < 0 ? 0 : sin(i / 4 / PI_F);
////        if (i > 10 && i < 50)
////            eqs[i] = 0;
////        qDebug() << "H[" << i << "]=" <<eqs[i];
//    }

//    EqualizerFilter eq;
//    HSFilter hsf(&eq);
//    hsf.setTH(15, 15, 13, 0.8f);
    PitchShiftFilter ps;
    ps.reloadSettings();

    QList<Filter*> filters;
    filters.append(&ps);

    QFile audio("in.wav");
    audio.open(QIODevice::ReadOnly);

    QFile phones("out.wav");
    phones.open(QIODevice::WriteOnly);

    QTime t;
    short tm = 0;
    while (!audio.atEnd())
    {
        QByteArray sam = audio.read(Filter::sample_length * sizeof(qint16));
        if (sam.length() == Filter::sample_length * sizeof(qint16))
        {
            t = QTime::currentTime();

            // Normalization
            /*
            qint16 *rawp = (qint16 *) sam.data();
            float sample[Filter::sample_length];
            float *fltp = sample;
            while ((char *) rawp < sam.data() + sam.length()) {
//                qDebug() << "I:" << *rawp;
                *fltp++ = (float) *rawp++ / 32768;
                qDebug() << "IN O:" << *fltp;
            }*/
//            hsf.process(sample);
//            eq.process(sample);
//            ps.process(sample);

//            rawp = (qint16 *) sam.data();
//            fltp = sample;
//            while ((char *) rawp < sam.data() + sam.length()) {
//                qDebug() << "OUT O:" << *fltp;
//                *rawp++ = (qint16) (*fltp++ * 32767);
//            }

            Filter::applyFilters(filters, sam);
            phones.write(sam);
            qDebug() << t.elapsed() << "passed" << tm++ * 32 << "ms";
        }
    }
    return 0;
}
