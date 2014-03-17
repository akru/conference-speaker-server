#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T18:47:00
#
#-------------------------------------------------

QT       -= gui

TARGET = Protocol
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    user_information.cpp \
    server_information.cpp \
    server_error.cpp

HEADERS += \
    user_information.h \
    server_information.h \
    cs_packet.h \
    server_error.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
