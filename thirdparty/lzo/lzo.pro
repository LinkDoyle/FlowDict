#-------------------------------------------------
#
# Project created by QtCreator 2016-07-29T22:35:34
#
#-------------------------------------------------

QT       -= core gui

TARGET = lzo
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    minilzo.c

HEADERS +=
unix {
    target.path = /usr/lib
    INSTALLS += target
}
