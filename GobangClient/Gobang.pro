#-------------------------------------------------
#
# Project created by QtCreator 2021-07-03T20:02:35
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Gobang
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        chessboard.cpp \
        gameai.cpp \
        gamecontrol.cpp \
        menu.cpp \
        main.cpp \
        netgame.cpp \
    netboard.cpp

HEADERS += \
        command.h\
        chessboard.h \
        gameai.h \
        gamecontrol.h \
        netgame.h \
        menu.h \
    netboard.h

FORMS += \
        chessboard.ui \
        gamecontrol.ui \
    menu.ui \
    netgame.ui \
    netboard.ui

RESOURCES += \
    picture.qrc

RC_ICONS = img\icon.ico
