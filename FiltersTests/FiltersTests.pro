#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T05:19:09
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_filterstests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_filterstests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Filters/release/ -lFilters
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Filters/debug/ -lFilters
else:unix: LIBS += -L$$OUT_PWD/../Filters/ -lFilters

INCLUDEPATH += $$PWD/../Filters
DEPENDPATH += $$PWD/../Filters

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Filters/release/libFilters.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Filters/debug/libFilters.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Filters/release/Filters.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Filters/debug/Filters.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Filters/libFilters.a
