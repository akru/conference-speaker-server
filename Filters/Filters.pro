#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T03:58:04
#
#-------------------------------------------------

QT       -= gui

TARGET = Filters
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    bandpass_filter.cpp \
    suppressor.cpp

HEADERS += filter.h \
    bandpass_filter.h \
    suppressor.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += $$PWD/../third-party/tspl_3_0/include
