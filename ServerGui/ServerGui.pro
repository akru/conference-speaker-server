QT       = core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = ServerGui
TEMPLATE = app

SOURCES  += main.cpp \
    main_window.cpp \
    settings.cpp \
    speaker_widget.cpp \
    vote_results_widget.cpp

HEADERS  += \
    main_window.h \
    settings.h \
    speaker_widget.h \
    vote_results_widget.h

FORMS    += \
    main_window.ui \
    speaker_widget.ui \
    vote_results_widget.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    csgui_en_US.ts \
    csgui_ru_RU.ts

## Include & link libraries

INCLUDEPATH += $$PWD/../ServerCore $$PWD/../Protocol $$PWD/../3rd-party/QRencode
DEPENDPATH += $$PWD/../ServerCore $$PWD/../Protocol $$PWD/../3rd-party/QRencode

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ServerCore/release/ -lServerCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ServerCore/debug/ -lServerCore
else:unix: LIBS += -L$$OUT_PWD/../ServerCore/ -lServerCore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Protocol/release/ -lProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Protocol/debug/ -lProtocol
else:unix: LIBS += -L$$OUT_PWD/../Protocol/ -lProtocol

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Suppression/release/ -lSuppression
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Suppression/debug/ -lSuppression
else:unix: LIBS += -L$$OUT_PWD/../Suppression/ -lSuppression

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/Soxr/release/ -lSoxr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/Soxr/debug/ -lSoxr
else:unix: LIBS += -L$$OUT_PWD/../3rd-party/Soxr/ -lSoxr

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/AppServer/release/ -lAppServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/AppServer/debug/ -lAppServer
else:unix: LIBS += -L$$OUT_PWD/../3rd-party/AppServer/ -lAppServer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/QRencode/release/ -lQRencode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rd-party/QRencode/debug/ -lQRencode
else:unix: LIBS += -L$$OUT_PWD/../3rd-party/QRencode/ -lQRencode
