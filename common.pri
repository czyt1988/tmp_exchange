CONFIG(debug, debug|release){
    contains(DEFINES, WIN64) {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_debug_64
    }else {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_debug
    }
}else{
    contains(DEFINES, WIN64) {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_release_64
    }else {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_release
    }
}
BIN_LIB_DIR = $$BIN_DIR/libs
