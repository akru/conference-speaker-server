#include "suppressor.h"

Suppressor::Suppressor(int trashold)
    : trashold(trashold)
{}

QByteArray Suppressor::process(QByteArray &data)
{
    qint16 *data_p = (qint16 *) data.data();
    // For all items of data
    while ((char *) data_p != data.data() + data.size())
        // When value below trashhold
        if (abs(*data_p) < trashold)
            // Replace by zero
            *data_p++ = 0;
    return data;
}
