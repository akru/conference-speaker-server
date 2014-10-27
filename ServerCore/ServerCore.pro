QT       = core network

TARGET   = ServerCore
TEMPLATE = lib
CONFIG  += staticlib

HEADERS += \
    broadcaster.h \
    connection.h \
    receiver.h \
    server.h \
    voting.h \
    recorder.h

SOURCES += \
    broadcaster.cpp \
    connection.cpp \
    receiver.cpp \
    server.cpp \
    voting.cpp \
    recorder.cpp

CONFIG(release, debug|release){
message("Release with licensing")

HEADERS += licensing.h
SOURCES += licensing.cpp

DEFINES     += NDEBUG

INCLUDEPATH += $$PWD/../../cryptopp
DEPENDPATH  += $$PWD/../../cryptopp
}

INCLUDEPATH += $$PWD/../Protocol $$PWD/../Speaker
DEPENDPATH  += $$PWD/../Protocol $$PWD/../Speaker

INCLUDEPATH += $$PWD/../Suppression
DEPENDPATH  += $$PWD/../Suppression

