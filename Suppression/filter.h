#ifndef FILTER_H
#define FILTER_H

#include <QByteArray>
#include <QString>
#include <QList>

// Audio filter interface
class Filter
{
public:
    virtual ~Filter() {}
    /*
     * Sample processing.
     * @param sample Is a normalized input sample with `sample_rate` rate
     * and `sample_length` length; [-1..1]
     */
    void process(float sample[]);
    /*
     * Overloaded processing method,
     * will be called from process method.
     */
    virtual void processFilter(float sample[]) = 0;
    /*
     * Filter name.
     */
    virtual QString name() = 0;
    /*
     * Settings reloader.
     */
    virtual void reloadSettings() = 0;
    /*
     * Settings filename getter.
     */
    QString settingsFiltename();
    /*
     * Enable filter.
     */
    void enable(bool e=true);

    // Sample params
    static const int sample_rate   = 22050; // Hz
    static const int sample_length = 512;  // Bins

    // Multiple filters manipulation
    /*
     * Apply multiple filters to raw sample.
     * @param filters Is a list of applyed filters.
     * @param raw_sample Is a sample to processing
     */
    static void applyFilters(QList<Filter*> &filters, QByteArray &raw_sample);
    /*
     * Reload settings for multiple filters.
     */
    static void reloadSettings(QList<Filter*> &filters);

protected:
    bool enabled;

    // Sample conversions
    static void fromPCM(qint16 pcm[], float sample[]);
    static void toPCM(float sample[], qint16 pcm[]);
};

#endif // FILTER_H
