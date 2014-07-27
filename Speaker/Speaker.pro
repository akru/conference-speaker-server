QT       += core multimedia

TARGET    = Speaker
TEMPLATE  = lib
CONFIG   += staticlib

SOURCES  += \
    speaker.cpp

HEADERS += \
    speaker.h \
    filter.h

CONFIG(debug): {
    QT      += gui
    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

    HEADERS += debug_dialog.h
    SOURCES += debug_dialog.cpp
    FORMS   += debug_dialog.ui
}

INCLUDEPATH += $$PWD/../Suppression
DEPENDPATH  += $$PWD/../Suppression
