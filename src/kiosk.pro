QT       = core gui network widgets multimedia webenginewidgets

CONFIG += console link_pkgconfig c++11
TARGET = kiosk
TEMPLATE = app

SOURCES += main.cpp\
    qplayer.cpp \
    KioskSettings.cpp \
    ElixirComs.cpp \
    KioskView.cpp \
    KioskMessage.cpp \
    Kiosk.cpp \
    KioskWindow.cpp

HEADERS  += \
    qplayer.h \
    KioskSettings.h \
    ElixirComs.h \
    KioskView.h \
    KioskMessage.h \
    Kiosk.h \
    KioskWindow.h

target.path = ../priv

INSTALLS += target

RESOURCES += \
    ui.qrc
