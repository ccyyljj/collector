#-------------------------------------------------
#
# Project created by QtCreator 2018-05-10T20:30:49
#
#-------------------------------------------------

QT       += core gui




greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

win32:LIBS += -lOpengl32 \
                -lglu32 \
                -lglut

LIBS+=-lws2_32
INCLUDEPATH += C:/WpdPack/Include
LIBS += -L C:/WpdPack/Lib/ -lwpcap -lpacket
DEFINES += WPCAP
DEFINES += HAVE_REMOTE

#include SP++
INCLUDEPATH += F:\SP++3.0\include
INCLUDEPATH += C:\fftw\include

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Collter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_OPENGL
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    getfpgadata.cpp \
    balisemanager.cpp \
    trafficgetter.cpp \
    qcustomplot.cpp \
    balisedemod.cpp \
    balisefre.cpp \
    pcaploop.cpp \
    dialog_changefreinfo.cpp \
    balisemsganalyse.cpp \
    mycustomplot.cpp \
    myTracer.cpp

HEADERS += \
        mainwindow.h \
    getfpgadata.h \
    balisemanager.h \
    balisestructdefine.h \
    trafficgetter.h \
    qcustomplot.h \
    balisedemod.h \
    balisefre.h \
    pcaploop.h \
    dialog_changefreinfo.h \
    balisemsganalyse.h \
    myTracer.h

FORMS += \
        mainwindow.ui \
    choosenetdialog.ui \
    dialog_changefreinfo.ui
