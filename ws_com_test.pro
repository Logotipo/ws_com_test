#-------------------------------------------------
#
# Project created by QtCreator 2023-12-26T08:41:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ws_com_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    consolemanager.cpp \
    modbusmaster.cpp \
    modbusmastersub.cpp \
    weatherstation.cpp

HEADERS += \
    consolemanager.h \
    modbus.h \
    modbusmaster.h \
    modbusmastersub.h \
    weatherstation.h
