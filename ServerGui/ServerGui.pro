QT       = core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = ServerGui
TEMPLATE = app

SOURCES  += main.cpp \
    main_window.cpp \
    about_dialog.cpp \
    channel_widget.cpp \
    settings_dialog.cpp \
    request_widget.cpp \
    vote_widget.cpp \
    client_widget.cpp \
    debug_dialog.cpp

HEADERS  += \
    main_window.h \
    about_dialog.h \
    channel_widget.h \
    settings_dialog.h \
    request_widget.h \
    vote_widget.h \
    client_widget.h \
    debug_dialog.h

FORMS    += \
    main_window.ui \
    about_dialog.ui \
    channel_widget.ui \
    settings_dialog.ui \
    request_widget.ui \
    vote_widget.ui \
    client_widget.ui \
    debug_dialog.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    csgui_en.ts \
    csgui_ru.ts

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ServerCore/release/ -lServerCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ServerCore/debug/ -lServerCore
else:unix: LIBS += -L$$OUT_PWD/../ServerCore/ -lServerCore

INCLUDEPATH += $$PWD/../ServerCore
DEPENDPATH += $$PWD/../ServerCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerCore/release/libServerCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerCore/debug/libServerCore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerCore/release/ServerCore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerCore/debug/ServerCore.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ServerCore/libServerCore.a

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Suppression/release/ -lSuppression
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Suppression/debug/ -lSuppression
else:unix: LIBS += -L$$OUT_PWD/../Suppression/ -lSuppression

INCLUDEPATH += $$PWD/../Suppression
DEPENDPATH += $$PWD/../Suppression

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/release/libSuppression.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/debug/libSuppression.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/release/Suppression.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/debug/Suppression.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Suppression/libSuppression.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/Soxr/release/ -lSoxr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/Soxr/debug/ -lSoxr
else:unix: LIBS += -L$$OUT_PWD/../3rd-party/Soxr/ -lSoxr
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rd-party/Soxr/release/libSoxr.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rd-party/Soxr/debug/libSoxr.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rd-party/Soxr/release/Soxr.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rd-party/Soxr/debug/Soxr.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../3rd-party/Soxr/libSoxr.a
