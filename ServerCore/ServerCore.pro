QT       = core network multimedia

TARGET   = ServerCore
TEMPLATE = lib
CONFIG  += staticlib

# Set the limit of connected users
DEFINES += "USER_LIMIT=50"

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

INCLUDEPATH += $$PWD/../Protocol $$PWD/../Suppression $$PWD/../3rd-party/Soxr
DEPENDPATH  += $$PWD/../Protocol $$PWD/../Suppression $$PWD/../3rd-party/Soxr
