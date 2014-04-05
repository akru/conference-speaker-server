#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T16:23:09
#
#-------------------------------------------------


QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app

SOURCES  += main.cpp \
    main_window.cpp \
    server.cpp \
    receiver.cpp \
    connection.cpp \
    broadcaster.cpp \
    about_dialog.cpp \
    channel_widget.cpp \
    settings_dialog.cpp \
    vote_dialog.cpp

HEADERS  += \
    main_window.h \
    server.h \
    receiver.h \
    connection.h \
    broadcaster.h \
    about_dialog.h \
    channel_widget.h \
    settings_dialog.h \
    vote_dialog.h

FORMS    += main_window.ui \
    about_dialog.ui \
    channel_widget.ui \
    settings_dialog.ui \
    vote_dialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Protocol/release/ -lProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Protocol/debug/ -lProtocol
else:unix: LIBS += -L$$OUT_PWD/../Protocol/ -lProtocol

INCLUDEPATH += $$PWD/../Protocol
DEPENDPATH += $$PWD/../Protocol


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
