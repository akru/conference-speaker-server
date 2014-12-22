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

INCLUDEPATH += $$PWD/../../Protocol $$PWD/../../ServerCore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Protocol/release/ -lProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Protocol/debug/ -lProtocol
else:unix: LIBS += -L$$OUT_PWD/../../Protocol/ -lProtocol
