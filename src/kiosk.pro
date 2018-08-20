QT       = core gui network widgets multimedia webenginewidgets

CONFIG += console link_pkgconfig c++11
CONFIG -= app_bundle

TARGET = kiosk
TEMPLATE = app

SOURCES += main.cpp\
    qplayer.cpp \
    KioskSettings.cpp \
    ElixirComs.cpp \
    KioskView.cpp \
    KioskMessage.cpp \
    Kiosk.cpp \
    KioskWindow.cpp \
    KioskProgress.cpp \
    Blanking.cpp

HEADERS  += \
    qplayer.h \
    KioskSettings.h \
    ElixirComs.h \
    KioskView.h \
    KioskMessage.h \
    Kiosk.h \
    KioskWindow.h \
    KioskProgress.h \
    Blanking.h

target.path = ../priv

INSTALLS += target

RESOURCES += \
    ui.qrc
