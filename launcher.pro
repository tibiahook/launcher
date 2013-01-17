TARGET = tibia-launcher

QT += core
QT -= gui
TEMPLATE = app
CONFIG += console

SOURCES += \
    src/Main.cpp \
    src/Injector.cpp \
    src/Initializer.cpp
HEADERS += \
    src/Initializer.h \
    src/Injector.h

include(../tibia-hook.pri)