QT      -= core gui

TARGET   = Soxr
TEMPLATE = lib
DEFINES += SOXR_DLL SOXR_LIB soxr_EXPORTS NDEBUG

SOURCES += \
    data-io.c \
    dbesi0.c \
    fft4g32.c \
    fft4g32s.c \
    fft4g64.c \
    fft4g.c \
    filter.c \
    rate32.c \
    rate32s.c \
    rate64.c \
    simd.c \
    soxr.c

HEADERS += \
    soxr.h \
    aliases.h \
    ccrw2.h \
    data-io.h \
    fft4g_cache.h \
    fft4g.h \
    fifo.h \
    filter.h \
    filters.h \
    half_coefs.h \
    half-fir.h \
    internal.h \
    poly-fir0.h \
    poly-fir.h \
    rate.h \
    rdft.h \
    rint-clip.h \
    rint.h \
    simd-dev.h \
    simd.h \
    soxr.h \
    soxr-config.h
