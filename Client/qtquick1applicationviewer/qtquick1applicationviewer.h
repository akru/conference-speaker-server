// checksum 0x7a92 version 0x90018
/*
  This file was generated by the Qt Quick 1 Application wizard of Qt Creator.
  QtQuick1ApplicationViewer is a convenience class containing mobile device
  specific code such as screen orientation handling. Also QML paths are
  handled here.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#ifndef QTQUICK1APPLICATIONVIEWER_H
#define QTQUICK1APPLICATIONVIEWER_H

#include <QDebug>

#include <QDeclarativeView>

class QtQuick1ApplicationViewer : public QDeclarativeView
{
    Q_OBJECT

public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit QtQuick1ApplicationViewer(QWidget *parent = 0);
    virtual ~QtQuick1ApplicationViewer();

    static QtQuick1ApplicationViewer *create();

    void setMainQmlFile(const QString &file);
    void addImportPath(const QString &path);

    // Note that this will only have an effect on Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

public slots:

private:
    class QtQuick1ApplicationViewerPrivate *d;
};

#endif // QTQUICK1APPLICATIONVIEWER_H
