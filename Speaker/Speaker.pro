QT       += core multimedia

QT       -= gui

TARGET    = Speaker
TEMPLATE  = lib
CONFIG   += staticlib

SOURCES  += \
    speaker.cpp \
    sample.cpp \

HEADERS += \
    speaker.h \
    filter.h \
    sample.h


INCLUDEPATH += $$PWD/../Suppression
DEPENDPATH  += $$PWD/../Suppression
