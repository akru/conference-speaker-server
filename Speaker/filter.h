#ifndef FILTER_H
#define FILTER_H

#include <QByteArray>
#include <QString>

// Audio filter interface
class Filter
{
public:
    virtual ~Filter() {}
    /*
     * Sample processing.
     * @param sample Is a normalized input sample with `sample_rate` rate
     * and `sample_length` length.
     */
    virtual void process(float sample[]) = 0;
    /*
     * Filter name.
     */
    virtual QString name() = 0;

    static const int sample_rate   = 8000; // Hz
    static const int sample_length = 256;  // Bins
};

#endif // FILTER_H
