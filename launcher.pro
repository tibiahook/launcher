include(../libraries.pri)

QT += core
QT -= gui

TEMPLATE = app
CONFIG += console

TARGET = tibia-injector

LIBS += \
    -L$${LIB_QT_JSON_BIN_DIR} \
    -l$${LIB_QT_JSON_BIN_NAME}

INCLUDEPATH += $${LIB_QT_JSON_SRC_DIR}

SOURCES += \
    src/Main.cpp \
    src/Initializer.cpp
HEADERS += \
    src/Injector.h \
    src/Initializer.h

unix {
    HEADERS += src/UnixInjector.h
    SOURCES += src/UnixInjector.cpp
}

win32 {
    HEADERS += src/WindowsInjector.h
    SOURCES += src/WindowsInjector.cpp
}

include(../shared.pri)
