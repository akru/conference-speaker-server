# Add more folders to ship with the application, here
qml_files.source = qml/Client
qml_files.target = qml
images.source = images
images.target = images
DEPLOYMENTFOLDERS = qml_files images

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT      += network

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

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick1applicationviewer/qtquick1applicationviewer.pri)
qtcAddDeployment()

RESOURCES += res.qrc

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
