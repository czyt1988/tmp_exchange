#-------------------------------------------------
#
# Project created by QtCreator 2021-04-17T09:15:28
#
#-------------------------------------------------

QT       += core gui xml printsupport
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += C++11
include($$PWD/common.pri)
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
    SACustomPlotAbstractValueTracer.cpp \
    SACustomPlotYValueTracer.cpp \
    main.cpp \
    GDataReviewWidget.cpp \
    GHvacIOManager.cpp \
    GPlotWidget.cpp \
    SACustomPlot.cpp \
    GNodeInfo.cpp \
    GTemplate.cpp \
    GIDUTableModel.cpp \
    GItemDisplayWidget.cpp \
    GModuleValueView.cpp \
    GHvacDataFileIO.cpp \
    SACsvStream.cpp \
    MainWindow.cpp \
    GHvacDataInfo.cpp \
    GFaultWidget.cpp \
    qcustomplot.cpp \
    GTemplateManager.cpp \
    SAColorList.cpp \
    SAColorMap.cpp \
    GBigDataAPI.cpp \
    GProjectArchivesWidget.cpp \
    GArchivesTableModel.cpp \
    SACustomPlotTreeModel.cpp

HEADERS += \
    GDataReviewWidget.h \
    GHvacIOManager.h \
    GPlotWidget.h \
    GNodeInfo.h \
    GTemplate.h \
    GIDUTableModel.h \
    SACustomPlot.h \
    SACustomPlotAbstractValueTracer.h \
    SACustomPlotYValueTracer.h \
    SAGlobals.h \
    SATable.h \
    GItemDisplayWidget.h \
    GModuleValueView.h \
    GHvacDataFileIO.h \
    SACsvStream.h \
    MainWindow.h \
    SAAlgorithm.h \
    GHvacDataInfo.h \
    GFaultWidget.h \
    qcustomplot.h \
    GTemplateManager.h \
    SAColorAlgorithm.h \
    SAColorList.h \
    SAColorMap.h \
    GBigDataAPI.h \
    GProjectArchivesWidget.h \
    GArchivesTableModel.h \
    SACustomPlotTreeModel.h

FORMS += \
    GDataReviewWidget.ui \
    GPlotWidget.ui \
    GItemDisplayWidget.ui \
    GModuleValueView.ui \
    MainWindow.ui \
    GFaultWidget.ui \
    GProjectArchivesWidget.ui

include($$PWD/3rdparty/quazip-0.7.3/use_quazip.pri)
include($$PWD/useSARibbonBar.pri)
#include($$PWD/3rdparty/QtXlsx/src/xlsx/qtxlsx.pri)
include($$PWD/3rdparty/QtXlsx/useQtXlsx.pri)
RESOURCES += \
    icon.qrc
