QT       += core multimedia

TARGET    = Speaker
TEMPLATE  = lib
CONFIG   += staticlib

SOURCES  += \
    speaker.cpp

HEADERS += \
    accbuffer.hpp \
    speaker.h \
    filter.h

INCLUDEPATH += $$PWD/../Suppression
DEPENDPATH  += $$PWD/../Suppression
