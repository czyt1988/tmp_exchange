#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

DEFINES += SA_RIBBON_BAR_MAKE_LIB #定义此宏将构建库
#DEFINES += SA_RIBBON_DEBUG_HELP_DRAW # 此宏将绘制辅助线用于调试
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = SARibbonBar
TEMPLATE = lib
include($$PWD/../common.pri)

DESTDIR = $${BIN_LIB_DIR}

include($$PWD/SARibbonBar.pri)

# 编译完成后自动把dll移动到$$BIN_DIR下面
win32 {
    PLUGINDLL_FULL_PATH = $${BIN_LIB_DIR}/$${TARGET}.dll
    PLUGINDLL_FULL_PATH = $$replace(PLUGINDLL_FULL_PATH, /, \\)
    PLUGINDLL_DESIGNER_PATH = $${BIN_DIR}/$${TARGET}.dll
    PLUGINDLL_DESIGNER_PATH = $$replace(PLUGINDLL_DESIGNER_PATH, /, \\)
    CMD_CPY = $${QMAKE_COPY} $${PLUGINDLL_FULL_PATH} $${PLUGINDLL_DESIGNER_PATH}
    message(cmd:$${CMD_CPY})
    QMAKE_POST_LINK += $${CMD_CPY}
}
