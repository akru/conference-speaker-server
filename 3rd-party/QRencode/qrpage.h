#ifndef QRPAGE_H
#define QRPAGE_H

#include "qrencode_global.h"
#include <QByteArray>
#include <QTextStream>

class QRENCODESHARED_EXPORT QRPage
{
public:
    QRPage();
    void printPage(const QString &addr,
                   const QString &androidAddr,
                   const QString &iosAddr,
                   const QString &h1,
                   const QString &h2);

private:
    void writeSVG_writeRect(QTextStream &svg, int x, int y, int width,
                            const QString &col, float opacity);
    QString writeSVG(const QString &string);
};

#endif // QRPAGE_H
