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
    channel_widget.cpp

HEADERS  += \
    main_window.h \
    server.h \
    receiver.h \
    connection.h \
    broadcaster.h \
    about_dialog.h \
    channel_widget.h

FORMS    += main_window.ui \
    about_dialog.ui \
    channel_widget.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Protocol/release/ -lProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Protocol/debug/ -lProtocol
else:unix: LIBS += -L$$OUT_PWD/../Protocol/ -lProtocol

INCLUDEPATH += $$PWD/../Protocol
DEPENDPATH += $$PWD/../Protocol

