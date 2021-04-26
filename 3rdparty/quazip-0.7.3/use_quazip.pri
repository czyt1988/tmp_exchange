INCLUDEPATH += $$PWD/quazip
DEPENDPATH += $$PWD/quazip
include($$PWD/../../common.pri)
CONFIG(debug, debug|release) {
    win32: LIB_NAME = quazipd
    mac: LIB_NAME = quazip_debug
} else {
    LIB_NAME = quazip
}
LIBS += -L$${BIN_LIB_DIR} -l$${LIB_NAME}
