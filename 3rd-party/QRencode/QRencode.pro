QT      += printsupport webkitwidgets

TARGET   = QRencode
TEMPLATE = lib

DEFINES += QRENCODE_LIBRARY

SOURCES += \
    bitstream.c \
    mask.c \
    mmask.c \
    mqrspec.c \
#    qrenc.c \
    qrencode.c \
    qrinput.c \
    qrspec.c \
    rscode.c \
    split.c \
    qrpage.cpp

HEADERS += \
    qrencode.h \
    qrencode_global.h \
    bitstream.h \
    mask.h \
    mmask.h \
    mqrspec.h \
    qrencode_inner.h \
    qrinput.h \
    qrspec.h \
    rscode.h \
    split.h \
    config.h \
    qrpage.h

DEFINES += \
    HAVE_CONFIG_H \
    HAVE_STDINT_H \
    HAVE_STDLIB_H \
    HAVE_STRING_H \
    MAJOR_VERSION=3 \
    MINOR_VERSION=4 \
    MICRO_VERSION=4
