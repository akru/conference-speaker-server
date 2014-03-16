#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T18:47:00
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = Protocol
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    control_packet.cpp \
    user_information.cpp \
    server_information.cpp

HEADERS += \
    control_packet.h \
    user_information.h \
    server_information.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
