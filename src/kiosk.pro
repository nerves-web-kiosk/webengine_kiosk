QT       = core gui network widgets multimedia webenginewidgets

CONFIG += console link_pkgconfig c++11
TARGET = kiosk
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    webview.cpp \
    qplayer.cpp \
    KioskSettings.cpp

HEADERS  += mainwindow.h \
    webview.h \
    qplayer.h \
    KioskSettings.h

target.path = ../priv

INSTALLS += target

RESOURCES += \
    ui.qrc
