QT       -= gui

TARGET    = Protocol
TEMPLATE  = lib
CONFIG   += staticlib

SOURCES  += \
    user_information.cpp \
    server_information.cpp \
    request.cpp \
    response.cpp \
    registration_request.cpp \
    channel_information.cpp \
    channel_response.cpp \
    voting_invite.cpp \
    app_information.cpp \
    documents_information.cpp

HEADERS  += \
    user_information.h \
    server_information.h \
    cs_packet.h \
    request.h \
    response.h \
    registration_request.h \
    channel_information.h \
    channel_response.h \
    voting_invite.h \
    app_information.h \
    documents_information.h
