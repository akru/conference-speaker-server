QT       -= gui

TARGET = Filters
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    suppressor.cpp \
    lms_filter.cpp \
    filter_thread.cpp

HEADERS += filter.h \
    suppressor.h \
    lms_filter.h \
    filter_thread.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
