QT     = core network multimedia

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
