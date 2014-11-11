QT       = core network multimedia

TARGET   = ServerCore
TEMPLATE = lib
CONFIG  += staticlib

HEADERS += \
    broadcaster.h \
    connection.h \
    receiver.h \
    server.h \
    voting.h \
    recorder.h \
    speaker.h

SOURCES += \
    broadcaster.cpp \
    connection.cpp \
    receiver.cpp \
    server.cpp \
    voting.cpp \
    recorder.cpp \
    speaker.cpp

CONFIG(release, debug|release){
message("Release with licensing")

HEADERS += licensing.h
SOURCES += licensing.cpp

DEFINES     += NDEBUG

INCLUDEPATH += $$PWD/../../cryptopp
DEPENDPATH  += $$PWD/../../cryptopp
}

INCLUDEPATH += $$PWD/../Protocol $$PWD/../Suppression $$PWD/../3rd-party/Soxr $$PWD/../3rd-party/AppServer
DEPENDPATH  += $$PWD/../Protocol $$PWD/../Suppression $$PWD/../3rd-party/Soxr $$PWD/../3rd-party/AppServer
