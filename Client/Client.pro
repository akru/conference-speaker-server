QT      += quick network

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    transmitter.cpp \
    discover.cpp \
    connector.cpp \
    client.cpp

HEADERS += \
    transmitter.h \
    discover.h \
    connector.h \
    client.h

RESOURCES += res.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Protocol/release/ -lProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Protocol/debug/ -lProtocol
else:unix: LIBS += -L$$OUT_PWD/../Protocol/ -lProtocol

INCLUDEPATH += $$PWD/../Protocol
DEPENDPATH += $$PWD/../Protocol
