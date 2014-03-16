#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T16:23:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app

SOURCES += main.cpp \
    main_window.cpp \
    server.cpp \
    receiver.cpp \
    connection.cpp \
    broadcaster.cpp

HEADERS  += \
    main_window.h \
    server.h \
    receiver.h \
    connection.h \
    broadcaster.h

FORMS    += main_window.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Protocol/release/ -lProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Protocol/debug/ -lProtocol
else:unix: LIBS += -L$$OUT_PWD/../Protocol/ -lProtocol

INCLUDEPATH += $$PWD/../Protocol
DEPENDPATH += $$PWD/../Protocol

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Protocol/release/libProtocol.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Protocol/debug/libProtocol.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Protocol/release/Protocol.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Protocol/debug/Protocol.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Protocol/libProtocol.a
