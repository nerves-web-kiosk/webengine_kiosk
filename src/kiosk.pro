QT       = core gui network widgets multimedia webenginewidgets

CONFIG += console link_pkgconfig c++11
TARGET = kiosk
TEMPLATE = app

SOURCES += main.cpp\
    MainWindow.cpp \
    WebView.cpp \
    qplayer.cpp \
    KioskSettings.cpp

HEADERS  += MainWindow.h \
    WebView.h \
    qplayer.h \
    KioskSettings.h

target.path = ../priv

INSTALLS += target

RESOURCES += \
    ui.qrc
