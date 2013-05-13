#-------------------------------------------------
#
# Project created by QtCreator 2013-04-25T09:39:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = multitouch
TEMPLATE = app


SOURCES +=  main.cpp\
            graphicsview.cpp \
            mainwindow.cpp

HEADERS +=  graphicsview.h \
            mainwindow.h

 # install
 target.path = $$[QT_INSTALL_EXAMPLES]/multitouch/pinchzoom
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS pinchzoom.pro images
 sources.path = $$[QT_INSTALL_EXAMPLES]/multitouch/pinchzoom
 INSTALLS += target sources
