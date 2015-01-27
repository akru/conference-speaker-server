QT       = core network multimedia

TARGET   = ServerCore
TEMPLATE = lib
CONFIG  += staticlib

HEADERS += \
    broadcaster.h \
    receiver.h \
    voting.h \
    recorder.h \
    speaker.h \
    gate.h \
    user.h

SOURCES += \
    broadcaster.cpp \
    receiver.cpp \
    voting.cpp \
    recorder.cpp \
    speaker.cpp \
    gate.cpp \
    user.cpp

CONFIG(release, debug|release){
message("Release with licensing")

HEADERS += licensing.h
SOURCES += licensing.cpp

DEFINES     += NDEBUG

INCLUDEPATH += $$PWD/../../cryptopp
DEPENDPATH  += $$PWD/../../cryptopp
}

INCLUDEPATH += $$PWD/../Protocol $$PWD/../Suppression $$PWD/../3rd-party/Soxr
DEPENDPATH  += $$PWD/../Protocol $$PWD/../Suppression $$PWD/../3rd-party/Soxr
