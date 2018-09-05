!versionAtLeast(QT_VERSION, 5.11) {
     message("Qt $${QT_VERSION} is too old. Try updating or manually specifying the location to qmake.")
     message("For example:")
     message("QMAKE=path/to/newer/qmake mix compile")
     return()
}

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

target.path = ../priv

INSTALLS += target

RESOURCES += \
    ui.qrc
