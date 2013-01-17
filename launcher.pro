TARGET = tibia-launcher

QT += core
QT -= gui
TEMPLATE = app
CONFIG += console

win32 {
    CONFIG += exceptions rtti
    DEFINES += WIN32
    DEFINES -= UNICODE
    QMAKE_CXXFLAGS += -U__STRICT_ANSI__
}

OUTPUTDIR = $$PWD

unix:DESTDIR=$${OUTPUTDIR}/build-unix/$${TARGET}
win32:DESTDIR=$${OUTPUTDIR}/build-win32/$${TARGET}

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

SOURCES += \
    src/Main.cpp \
    src/Injector.cpp \
    src/Initializer.cpp
HEADERS += \
    src/Initializer.h \
    src/Injector.h
