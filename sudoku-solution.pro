#-------------------------------------------------
#
# Project created by QtCreator 2016-03-12T22:10:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudoku-qt
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    mixingVariants.cpp \
    modelEntities.cpp \
    sudokumodel.cpp \
    sudokusolvechecker.cpp

HEADERS  += \
    constants.h \
    mainwindow.h \
    mixingVariants.h \
    modelEntities.h \
    sudokumodel.h \
    sudokusolvechecker.h

FORMS    +=
