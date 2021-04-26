CONFIG(debug, debug|release){
    contains(QT_ARCH, i386) {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_debug
    }else {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_debug_64
    }
}else{
    contains(QT_ARCH, i386) {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_release
    }else {
        BIN_DIR = $$PWD/../bin_qt$$[QT_VERSION]_release_64
    }
}
BIN_LIB_DIR = $$BIN_DIR/libs
