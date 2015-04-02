#-------------------------------------------------
#
# Project created by QtCreator 2015-03-27T15:42:08
#
#-------------------------------------------------

#Remember to remove the TEST define below to run functionality on the Pi

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

target.path = /home/pi/
TARGET = Eyebot
TEMPLATE = app
INSTALLS += target

SOURCES += main.cpp\
        mainwindow.cpp\
        home.cpp \
        system.cpp \
    network.cpp \
    settings.cpp \
    commands.cpp \
    hardware.cpp \
    software.cpp \
    old_code/src/hdt.c \
    old_code/src/mpsse.c \

HEADERS  += mainwindow.h\
            home.h \
            system.h \
    network.h \
    settings.h \
    commands.h \
    hardware.h \
    software.h \
    old_code/include/hdt.h \
    old_code/include/Xlib.h \
    old_code/include/ftdi.h \
    old_code/include/mpsse.h \
    old_code/include/Xlib.h \
    old_code/include/usb.h \
    old_code/include/types.h

FORMS    += mainwindow.ui

#DEFINES += TEST

