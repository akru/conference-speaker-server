QT      -= gui

TARGET   = AppServer
TEMPLATE = lib

DEFINES += APPSERVER_LIBRARY

SOURCES += \
    app_server.cpp \
    mongoose.c

HEADERS += \
    appserver_global.h \
    app_server.h \
    mongoose.h

DEFINES += \
    MONGOOSE_NO_FILESYSTEM \
    MONGOOSE_NO_WEBSOCKET
