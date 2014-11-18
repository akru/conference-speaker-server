#include "qrpage.h"
#include "qrencode.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QWebView>
#include <QFile>

#include <QDebug>

#define INCHES_PER_METER (100.0f/2.54f)
static const int dpi = 72;
static const QRecLevel level = QR_ECLEVEL_L;
static const unsigned int fg_color[4] = {0, 0, 0, 255};
static const unsigned int bg_color[4] = {255, 255, 255, 255};
static const int margin = 2;
static const int rle = 0;
static const int size = 5;

QRPage::QRPage()
{
}

void QRPage::printPage(const QString &addr,
                       const QString &androidAddr,
                       const QString &iosAddr,
                       const QString &h1,
                       const QString &h2)
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer);
    if (dialog.exec())
    {
        printer.setFullPage(true);
        QFile ix(":/qr-code/index.html"); ix.open(QIODevice::ReadOnly);
        QString page = QString::fromLatin1(ix.readAll())
                .arg(h1, writeSVG(androidAddr), writeSVG(iosAddr), addr, h2);
        QWebView view;
        view.setHtml(page);
        view.print(&printer);
    }
}

QString QRPage::writeSVG(const QString &string)
{
    unsigned char *row, *p;
    int x, y, x0, pen;
    int symwidth, realwidth;
    float scale;
    QString fg, bg;
    float fg_opacity;
    float bg_opacity;
    QString svgBuf;
    QTextStream svg(&svgBuf);

    QByteArray latin1string = string.toLatin1();
    QRcode *qrcode = QRcode_encodeData(latin1string.size(), (unsigned char *)latin1string.data(), 0, level);
    if (!qrcode)
    {
        qWarning() << "No valid QR code!";
        return "ERROR";
    }

    scale = dpi * INCHES_PER_METER / 100.0f;

    symwidth = qrcode->width + margin * 2;
    realwidth = symwidth * size;

    fg = QString("%1%2%3")
            .arg(fg_color[0], 1, 16)
            .arg(fg_color[1], 1, 16)
            .arg(fg_color[2], 1, 16)
            .toUpper();
    bg = QString("%1%2%3")
            .arg(bg_color[0], 1, 16)
            .arg(bg_color[1], 1, 16)
            .arg(bg_color[2], 1, 16)
            .toUpper();
    fg_opacity = (float)fg_color[3] / 255;
    bg_opacity = (float)bg_color[3] / 255;

    /* XML declaration */
    //svg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";

    /* DTD
       No document type specified because "while a DTD is provided in [the SVG]
       specification, the use of DTDs for validating XML documents is known to be
       problematic. In particular, DTDs do not handle namespaces gracefully. It
       is *not* recommended that a DOCTYPE declaration be included in SVG
       documents."
       http://www.w3.org/TR/2003/REC-SVG11-20030114/intro.html#Namespace
    */

    /* SVG code start */
    svg << QString("<svg width=\"%1cm\" height=\"%2cm\" viewBox=\"0 0 %3 %4\""\
            " preserveAspectRatio=\"none\" version=\"1.1\""\
            " xmlns=\"http://www.w3.org/2000/svg\">\n")
           .arg(realwidth / scale)
           .arg(realwidth / scale)
           .arg(symwidth).arg(symwidth);

    /* Make named group */
    svg << "\t<g id=\"QRcode\">\n";

    /* Make solid background */
    if(bg_color[3] != 255) {
        svg << QString("\t\t<rect x=\"0\" y=\"0\" width=\"%1\" height=\"%2\" fill=\"#%3\" fill-opacity=\"%4\" />\n")
               .arg(symwidth).arg(symwidth).arg(bg).arg(bg_opacity);
    } else {
        svg << QString("\t\t<rect x=\"0\" y=\"0\" width=\"%1\" height=\"%2\" fill=\"#%3\" />\n")
               .arg(symwidth).arg(symwidth).arg(bg);
    }

    /* Create new viewbox for QR data */
    svg << "\t\t<g id=\"Pattern\">\n";

    /* Write data */
    p = qrcode->data;
    for(y=0; y<qrcode->width; y++) {
        row = (p+(y*qrcode->width));

        if( !rle ) {
            /* no RLE */
            for(x=0; x<qrcode->width; x++) {
                if(*(row+x)&0x1) {
                    writeSVG_writeRect(svg,	margin + x,
                                margin + y, 1,
                                fg, fg_opacity);
                }
            }
        } else {
            /* simple RLE */
            pen = 0;
            x0  = 0;
            for(x=0; x<qrcode->width; x++) {
                if( !pen ) {
                    pen = *(row+x)&0x1;
                    x0 = x;
                } else {
                    if(!(*(row+x)&0x1)) {
                        writeSVG_writeRect(svg, x0 + margin, y + margin, x-x0, fg, fg_opacity);
                        pen = 0;
                    }
                }
            }
            if( pen ) {
                writeSVG_writeRect(svg, x0 + margin, y + margin, qrcode->width - x0, fg, fg_opacity);
            }
        }
    }
    /* Close QR data viewbox */
    svg << "\t\t</g>\n";

    /* Close group */
    svg << "\t</g>\n";

    /* Close SVG code */
    svg << "</svg>\n";

    QRcode_free(qrcode);
    return svgBuf.toLatin1();
}

void QRPage::writeSVG_writeRect(QTextStream &svg, int x, int y, int width,
                                const QString &col, float opacity)
{
    if(fg_color[3] != 255) {
        svg << QString("\t\t\t<rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"1\" "\
                "fill=\"#%4\" fill-opacity=\"%5\" />\n")
                .arg(x).arg(y).arg(width).arg(col).arg(opacity);
    } else {
        svg << QString("\t\t\t<rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"1\" "\
                "fill=\"#%4\" />\n")
                .arg(x).arg(y).arg(width).arg(col);
    }
}
