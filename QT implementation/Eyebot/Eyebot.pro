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
        info.cpp

HEADERS  += mainwindow.h\
            home.h \
            info.h

FORMS    += mainwindow.ui

DEFINES +=
