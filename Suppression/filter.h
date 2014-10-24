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

protected:
    bool enabled;
};

#endif // FILTER_H
