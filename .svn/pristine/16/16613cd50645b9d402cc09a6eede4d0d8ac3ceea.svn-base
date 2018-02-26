#-------------------------------------------------
#
# Project created by QtCreator 2017-02-09T10:04:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport charts

TARGET = "MicroServo Studio"
CONFIG(debug ,debug|release){
TARGET = "MicroServo Studio_lts"
}
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


SOURCES += main.cpp\
        innfoswindow.cpp \
    innfostoolbar.cpp \
    paramwidget.cpp \
    framelesshelper.cpp \
    communication.cpp \
    idata.cpp \
    innfosWizard.cpp \
    mediator.cpp \
    innfosproxy.cpp \
    autorecoginze.cpp \
    proxyparser.cpp \
    motorform.cpp \
    motordata.cpp \
    dataUtil.cpp \
    innfoschartwidget.cpp \
    timeruler.cpp \
    actiondelegate.cpp \
    actionitem.cpp \
    editorwidget.cpp \
    inertiamove.cpp \
    actiontree.cpp \
    innfoschartview.cpp \
    callout.cpp \
    wavetriggerwidget.cpp \
    actioncmds.cpp \
    angleclock.cpp \
    mydoublespinbox.cpp \
    warningdialog.cpp \
    msgbox.cpp \
    filter.cpp \
    flowlayout.cpp \
    launchwindow.cpp

HEADERS  += innfoswindow.h \
    innfostoolbar.h \
    innfosutil.h \
    paramwidget.h \
    framelesshelper.h \
    communication.h \
    idata.h \
    innfosWizard.h \
    mediator.h \
    innfosproxy.h \
    autorecoginze.h \
    proxyparser.h \
    motorform.h \
    motordata.h \
    dataUtil.h \
    innfoschartwidget.h \
    timeruler.h \
    actiondelegate.h \
    actionitem.h \
    editorwidget.h \
    inertiamove.h \
    actiontree.h \
    innfoschartview.h \
    callout.h \
    wavetriggerwidget.h \
    actioncmds.h \
    angleclock.h \
    mydoublespinbox.h \
    warningdialog.h \
    msgbox.h \
    filter.h \
    flowlayout.h \
    launchwindow.h

RESOURCES += \
    images.qrc \
    qss.qrc

FORMS += \
    motorform.ui \
    filter.ui \
    launchwindow.ui

CONFIG(release ,debug|release){
    DEFINES += NO_CALIBRATION #use to switch calibration func
    DEFINES += RELEASE_LOG #use to record commands that wait to send
}


VERSION = 1.0.0
RC_ICONS = images/000002.ico
