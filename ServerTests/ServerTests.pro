QT       += core network multimedia
QT       -= gui

TARGET    = ServerTests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE  = app


SOURCES  += \
    main.cpp \
    discover.cpp \
    connector.cpp \
    transmitter.cpp

HEADERS += \
    discover.h \
    connector.h \
    transmitter.h

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
