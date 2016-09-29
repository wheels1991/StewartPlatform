#-------------------------------------------------
#
# Project created by QtCreator 2016-03-14T16:50:45
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StewartPlatform
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Platform.cpp \
    SixJoints.cpp \
    SerialPort.cpp

HEADERS  += MainWindow.h \
    Platform.h \
    SixJoints.h \
    SerialPort.h

FORMS    += MainWindow.ui \
    SixJoints.ui \
    SerialPort.ui
