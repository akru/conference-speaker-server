QT       += core multimedia

TARGET    = Speaker
TEMPLATE  = lib
CONFIG   += staticlib

SOURCES  += \
    speaker.cpp \
    processing.cpp

HEADERS += \
    accbuffer.hpp \
    speaker.h \
    processing.h

INCLUDEPATH += $$PWD/../Suppression $$PWD/../3rd-party/Soxr
DEPENDPATH  += $$PWD/../Suppression $$PWD/../3rd-party/Soxr
