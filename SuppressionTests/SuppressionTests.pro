QT      += core
QT      -= gui

TARGET   = SuppressionTests
CONFIG  += console
CONFIG  -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Suppression/release/ -lSuppression
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Suppression/debug/ -lSuppression
else:unix: LIBS += -L$$OUT_PWD/../Suppression/ -lSuppression

INCLUDEPATH += $$PWD/../Suppression
DEPENDPATH  += $$PWD/../Suppression

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/release/libSuppression.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/debug/libSuppression.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/release/Suppression.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Suppression/debug/Suppression.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Suppression/libSuppression.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Speaker/release/ -lSpeaker
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Speaker/debug/ -lSpeaker
else:unix: LIBS += -L$$OUT_PWD/../Speaker/ -lSpeaker

INCLUDEPATH += $$PWD/../Speaker
DEPENDPATH  += $$PWD/../Speaker

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/release/libSpeaker.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/debug/libSpeaker.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/release/Speaker.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Speaker/debug/Speaker.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Speaker/libSpeaker.a
