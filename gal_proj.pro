#-------------------------------------------------
#
# Project created by QtCreator 2014-11-26T10:42:44
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gal_proj
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myqgraphicsview.cpp \
    gplace.cpp \
    gedge.cpp

HEADERS  += mainwindow.h \
    myqgraphicsview.h \
    gplace.h \
    gedge.h

FORMS    += mainwindow.ui
