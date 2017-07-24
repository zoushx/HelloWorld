TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../FFmpegDecoder.cpp \
    ../FFmpegH264Encoder.cpp \
    ../FFmpegH264Source.cpp \
    ../LiveRTSPServer.cpp \
    ../LiveServerMediaSubsession.cpp \
    ../main.cpp

HEADERS += \
    ../FFmpegDecoder.h \
    ../FFmpegH264Encoder.h \
    ../FFmpegH264Source.h \
    ../LiveRTSPServer.h \
    ../LiveServerMediaSubsession.h

