QT       -= gui

TARGET    = Suppression
TEMPLATE  = lib
CONFIG   += staticlib

SOURCES  += \
    noise_suppression.c \
    ns_core.c \
#    howling_suppression.c \
    ns_filter.cpp \
    hs_filter.cpp \
#    biquad.c \
#    bandswitch_filter.cpp \
#    highpass_filter.cpp \
    compressor_filter.cpp \
    equalizer_filter.cpp \
    pitch_shift_filter.cpp \
    gate_filter.cpp \
    agc_filter.cpp \
    filter.cpp \
    ampanalyze_filter.cpp

HEADERS  += \
    noise_suppression.h \
    ns_core.h \
    defines.h \
    windows_private.h \
    typedefs.h \
    signal_processing_library.h \
#    howling_suppression.h \
    ns_filter.h \
    hs_filter.h \
#    biquad.h \
#    bandswitch_filter.h \
#    highpass_filter.h \
    compressor_filter.h \
    equalizer_filter.h \
    pitch_shift_filter.h \
    gate_filter.h \
    agc_filter.h \
    filter.h \
    ampanalyze_filter.h

INCLUDEPATH += $$PWD/../3rd-party/Soxr
DEPENDPATH += $$PWD/../3rd-party/Soxr
