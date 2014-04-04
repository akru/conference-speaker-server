#-------------------------------------------------
#
# Project created by QtCreator 2014-04-04T22:13:16
#
#-------------------------------------------------

QT       -= gui

TARGET = AEC
TEMPLATE = lib
CONFIG += staticlib

SOURCES += aec.cpp

HEADERS += aec.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
