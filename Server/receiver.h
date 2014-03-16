#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>

class Receiver : public QObject
{
    Q_OBJECT
public:
    Receiver(QObject *parent = 0);
};

#endif // RECEIVER_H
