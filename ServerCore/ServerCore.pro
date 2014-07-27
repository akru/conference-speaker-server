QT       = core network

TARGET   = ServerCore
TEMPLATE = lib
CONFIG  += staticlib

HEADERS += \
    broadcaster.h \
    connection.h \
    receiver.h \
    server.h

SOURCES += \
    broadcaster.cpp \
    connection.cpp \
    receiver.cpp \
    server.cpp

macx {
    DEFINES += MACOSX
}


CONFIG(debug): {
    QT      += gui
    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
}

INCLUDEPATH += $$PWD/../Protocol $$PWD/../Speaker
DEPENDPATH  += $$PWD/../Protocol $$PWD/../Speaker
