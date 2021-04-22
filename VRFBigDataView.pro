#-------------------------------------------------
#
# Project created by QtCreator 2021-04-17T09:15:28
#
#-------------------------------------------------

QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include($$PWD/../common.pri)
TARGET = VRFBigDataView
TEMPLATE = app
DESTDIR = $${BIN_DIR}
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWidget.cpp \
    GNodeInfo.cpp \
    GTemplate.cpp \
    GIDUTableModel.cpp \
    GItemDisplayWidget.cpp \
    GModuleValueView.cpp \
    GHvacDataFileIO.cpp \
    SACsvStream.cpp

HEADERS += \
        MainWidget.h \
    GNodeInfo.h \
    GTemplate.h \
    GIDUTableModel.h \
    SATable.h \
    GItemDisplayWidget.h \
    GModuleValueView.h \
    GHvacDataFileIO.h \
    SACsvStream.h

FORMS += \
        MainWidget.ui \
    GItemDisplayWidget.ui \
    GModuleValueView.ui

include($$PWD/../quazip-0.7.3/use_quazip.pri)
