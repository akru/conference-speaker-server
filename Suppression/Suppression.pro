QT       -= gui

TARGET    = Suppression
TEMPLATE  = lib
CONFIG   += staticlib

QMAKE_CFLAGS += -O3 -std=c99

SOURCES  += \
    noise_suppression.c \
    ns_core.c \
    fft4g.c \
    howling_suppression.c \
    ns_filter.cpp \
    hs_filter.cpp

HEADERS  += \
    noise_suppression.h \
    ns_core.h \
    defines.h \
    windows_private.h \
    typedefs.h \
    fft4g.h \
    signal_processing_library.h \
    howling_suppression.h \
    ns_filter.h \
    hs_filter.h
