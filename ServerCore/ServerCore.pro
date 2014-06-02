QT     = core network

TARGET = ServerCore
TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    broadcaster.h \
    connection.h \
    receiver.h \
    server.h

SOURCES += \
    broadcaster.cpp \
    connection.cpp \
    receiver.cpp \
    server.cpp

macx {
    DEFINES += MACOSX
}

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Speaker/release/ -lSpeaker
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Speaker/debug/ -lSpeaker
else:unix: LIBS += -L$$OUT_PWD/../Speaker/ -lSpeaker

INCLUDEPATH += $$PWD/../Speaker
DEPENDPATH += $$PWD/../Speaker

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/release/libSpeaker.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/debug/libSpeaker.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/release/Speaker.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/debug/Speaker.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Speaker/libSpeaker.a
