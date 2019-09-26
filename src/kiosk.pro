QT       = core gui network widgets multimedia webenginewidgets

CONFIG += console link_pkgconfig c++11
CONFIG -= app_bundle

TARGET = kiosk
TEMPLATE = app

SOURCES += main.cpp\
    KioskSettings.cpp \
    ElixirComs.cpp \
    KioskView.cpp \
    KioskMessage.cpp \
    Kiosk.cpp \
    KioskWindow.cpp \
    KioskProgress.cpp \
    Blanking.cpp \
    KioskSounds.cpp \
    StderrPipe.cpp

HEADERS  += \
    KioskSettings.h \
    ElixirComs.h \
    KioskView.h \
    KioskMessage.h \
    Kiosk.h \
    KioskWindow.h \
    KioskProgress.h \
    Blanking.h \
    KioskSounds.h \
    StderrPipe.h

RESOURCES += \
    ui.qrc

# The following line requires $INSTALL_ROOT to be set to $MIX_APP_PATH when
# calling "make install". See $MIX_APP_PATH/obj/Makefile.
target.path = /priv

INSTALLS += target

