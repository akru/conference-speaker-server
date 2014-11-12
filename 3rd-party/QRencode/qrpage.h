#ifndef QRPAGE_H
#define QRPAGE_H

#include "qrencode_global.h"
#include <QByteArray>
#include <QTextStream>

class QRENCODESHARED_EXPORT QRPage
{
public:
    QRPage();
    void printPage(const QString &string);

private:
    void writeSVG_writeRect(QTextStream &svg, int x, int y, int width,
                            const QString &col, float opacity);
    QByteArray writeSVG(const QString &string);
};

#endif // QRPAGE_H
