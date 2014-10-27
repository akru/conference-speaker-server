#include "processing.h"

#include <ns_filter.h>
#include <hs_filter.h>
#include <agc_filter.h>
#include <gate_filter.h>
#include <equalizer_filter.h>
#include <compressor_filter.h>
#include <ampanalyze_filter.h>
#include <pitch_shift_filter.h>

static const int max_qint16 = 32768;

Processing::Processing()
    : gScaler(1.0f)
{
    // Append filters
    filters.append(new AmpAnalyzeFilter); // WARNING: Must be first !!!
    filters.append(new AGCFilter);
    filters.append(new NSFilter);
    filters.append(new GateFilter);
    filters.append(new CompressorFilter);
    EqualizerFilter *eq = new EqualizerFilter;
    filters.append(new HSFilter(eq));
    filters.append(eq);
    filters.append(new PitchShiftFilter);
}

Processing::~Processing()
{
    foreach (Filter *f, filters) {
        delete f;
    }
}

void Processing::insert(const QByteArray &packet)
{
    acc.putData((qint16 *) packet.data(),
                packet.size() / sizeof(qint16));
}

QByteArray Processing::take()
{
    QByteArray sample_raw(Filter::sample_length * sizeof(qint16),
                          Qt::Uninitialized);
    // When accumulator has too low sounds - skips
    if (!acc.avail(Filter::sample_length * sizeof(qint16) * 2))
        return QByteArray();
    // Drop too old samples from acc
    acc.purity(Filter::sample_length * sizeof(qint16) * 4);
    // Prepare sample
    acc.getData((qint16 *) sample_raw.data(), Filter::sample_length);
    // Apply filters
    applyFilters(sample_raw);
    // Store analyzed amplitude
    avgAmp = ((AmpAnalyzeFilter *) filters.first())->getAmp();
    // Return processed sample
    return sample_raw;
}

void Processing::reloadSettings()
{
    foreach (Filter *f, filters) {
        f->reloadSettings();
    }
}

void Processing::applyFilters(QByteArray &raw_sample)
{
    float sample[Filter::sample_length];
    fromPCM((qint16 *)raw_sample.data(), sample);
    foreach (Filter *f, filters) {
        f->process(sample);
    }
    toPCM(sample, (qint16 *)raw_sample.data());
}

void Processing::fromPCM(qint16 pcm[], float sample[])
{
    // Normalization & scaling
    for (short i = 0; i < Filter::sample_length; ++i)
        sample[i] = gScaler * pcm[i] / max_qint16;
}

void Processing::toPCM(float sample[], qint16 pcm[])
{
    // Back to the RAW
    bool distortion = false;
    for (short i = 0; i < Filter::sample_length; ++i) {
        if(sample[i] > 1.0)
            distortion = true;
        if(distortion)
            qDebug() << "Distortion detected!";
        pcm[i] = fabs(sample[i]) >= 1.0
                  ? sample[i] / sample[i] * (max_qint16 - 10)
                  : sample[i] * max_qint16;
    }
}

QByteArray Processing::mix(const QByteArray &s1,
                           const QByteArray &s2)
{
    Q_ASSERT(s1.size() == s2.size());
    // Initialize data and pointers
    QByteArray sample_out(s1.size(), Qt::Uninitialized);
    qint16 *inp1 = (qint16 *) s1.data(),
           *inp2 = (qint16 *) s2.data(),
           *outp = (qint16 *) sample_out.data();
    int max_amp = 0;
    // First step (simple sum & calc max amp)
    while ((char *)outp < sample_out.data() + sample_out.size())
    {
        if (max_amp < abs((int)*inp1 + (int)*inp2))
            max_amp = abs((int)*inp1 + (int)*inp2);
        *outp++ = *inp1++ + *inp2++;
    }
    // Second step (when overdrive is detected)
    if (max_amp > max_qint16)
    {
        inp1 = (qint16 *) s1.data();
        inp2 = (qint16 *) s2.data();
        outp = (qint16 *) sample_out.data();
        const float scaler = 1.0f * max_qint16 / max_amp;
        while ((char *) outp < sample_out.data() + sample_out.size())
            *outp++ = *inp1++ * scaler + *inp2++ * scaler;
    }
    return sample_out;
}
