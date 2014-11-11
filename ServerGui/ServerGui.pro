QT       = core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = ServerGui
TEMPLATE = app

SOURCES  += main.cpp \
    main_window.cpp \
    settings.cpp \
    speaker_widget.cpp

HEADERS  += \
    main_window.h \
    settings.h \
    speaker_widget.h

FORMS    += \
    main_window.ui \
    speaker_widget.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    csgui_en.ts \
    csgui_ru.ts

## Include & link libraries

INCLUDEPATH += $$PWD/../ServerCore $$PWD/../Protocol
DEPENDPATH += $$PWD/../ServerCore $$PWD/../Protocol

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ServerCore/release/ -lServerCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ServerCore/debug/ -lServerCore
else:unix: LIBS += -L$$OUT_PWD/../ServerCore/ -lServerCore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Protocol/release/ -lProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Protocol/debug/ -lProtocol
else:unix: LIBS += -L$$OUT_PWD/../Protocol/ -lProtocol

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Speaker/release/ -lSpeaker
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Speaker/debug/ -lSpeaker
else:unix: LIBS += -L$$OUT_PWD/../Speaker/ -lSpeaker

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Suppression/release/ -lSuppression
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Suppression/debug/ -lSuppression
else:unix: LIBS += -L$$OUT_PWD/../Suppression/ -lSuppression

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/Soxr/release/ -lSoxr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/Soxr/debug/ -lSoxr
else:unix: LIBS += -L$$OUT_PWD/../3rd-party/Soxr/ -lSoxr

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/AppServer/release/ -lAppServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/AppServer/debug/ -lAppServer
else:unix: LIBS += -L$$OUT_PWD/../3rd-party/AppServer/ -lAppServer
