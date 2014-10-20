QT       += core multimedia

TARGET    = Speaker
TEMPLATE  = lib
CONFIG   += staticlib

SOURCES  += \
    speaker.cpp

HEADERS += \
    accbuffer.hpp \
    speaker.h

INCLUDEPATH += $$PWD/../Suppression $$PWD/../3rd-party/Soxr
DEPENDPATH  += $$PWD/../Suppression $$PWD/../3rd-party/Soxr
