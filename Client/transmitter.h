#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QObject>

class Transmitter : public QObject
{
    Q_OBJECT
public:
    explicit Transmitter(QObject *parent = 0);

signals:

public slots:

};

#endif // TRANSMITTER_H
