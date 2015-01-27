QT      -= gui

TARGET   = AppServer
TEMPLATE = lib
CONFIG  += staticlib

DEFINES += APPSERVER_LIBRARY

SOURCES += \
    app_server.cpp \
    mongoose.c \
    user_adapter.cpp

HEADERS += \
    app_server.h \
    mongoose.h \
    user_adapter.h

DEFINES += \
    MONGOOSE_NO_FILESYSTEM \
    MONGOOSE_NO_WEBSOCKET

INCLUDEPATH += $$PWD/../../Protocol $$PWD/../../ServerCore
