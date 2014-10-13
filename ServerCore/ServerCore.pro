QT       = core network

TARGET   = ServerCore
TEMPLATE = lib
CONFIG  += staticlib

HEADERS += \
    broadcaster.h \
    connection.h \
    receiver.h \
    server.h \
    licensing.h

SOURCES += \
    broadcaster.cpp \
    connection.cpp \
    receiver.cpp \
    server.cpp \
    licensing.cpp

INCLUDEPATH += $$PWD/../Protocol $$PWD/../Speaker
DEPENDPATH  += $$PWD/../Protocol $$PWD/../Speaker

INCLUDEPATH += $$PWD/../Suppression
DEPENDPATH  += $$PWD/../Suppression

DEFINES     += NDEBUG

INCLUDEPATH += $$PWD/../../cryptopp
DEPENDPATH  += $$PWD/../../cryptopp
