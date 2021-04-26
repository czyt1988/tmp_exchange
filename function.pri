################################################################
# 用于拷贝编译完成的库到对应目录
# czy.t@163.com
# 尘中远于2021年，添加快速部署脚本
################################################################
include( $${PWD}/common.pri )

# 用于修正路径，对于window会把路径的/转变为\,unix下不作为
defineReplace(saFixPath) {
    PathName = $$1
    win32 {
        FixPathName = $$replace(PathName, /, \\)
    }
    unix {
        FixPathName = $${PathName}
    }
    return ($${FixPathName})
}

# 生成拷贝cmd命令
defineReplace(saCopyLibCMD) {
    LibName = $$1
    win32 {
        DIR1 = $${BIN_LIB_DIR}/$${LibName}.dll
    }
    unix {
        DIR1 = $${BIN_LIB_DIR}/$${LibName}.so
    }
    DIR2 = $${BIN_DIR}/
    DIR_FROM = $$saFixPath($${DIR1})
    DIR_TO = $$saFixPath($${DIR2})
    CMD_CPY = $${QMAKE_COPY} $${DIR_FROM} $${DIR_TO}
    return ($${CMD_CPY})
}

# 生成拷贝第三方库的cmd命令
defineReplace(saCopyFullPathCMD) {
    LibFullPath = $$1
    LibName = $$2
    win32 {
        DIR1 = $${LibFullPath}/$${LibName}.dll
    }
    unix {
        DIR1 = $${LibFullPath}/$${LibName}.so
    }
    DIR2 = $${BIN_DIR}/
    DIR_FROM = $$saFixPath($${DIR1})
    DIR_TO = $$saFixPath($${DIR2})
    CMD_CPY = $${QMAKE_COPY} $${DIR_FROM} $${DIR_TO}
    return ($${CMD_CPY})
}

# 生成拷贝第三方库的cmd命令 arg1: lib名称 此函数将从$$BIN_LIB_DIR的lib文件拷贝到$$BIN_DIR下
defineReplace(saCopyLibToBin) {
    CMD = $$saCopyLibCMD($$1)
    QMAKE_POST_LINK += $${CMD}
    export(QMAKE_POST_LINK)
}

# 生成拷贝第三方库的cmd命令 arg1: lib路径，arg2:lib名称 此函数需要自己定义lib路径，将拷贝到$$BIN_DIR下
defineReplace(saCopyFullPathLibToBin) {
    CMD = $$saCopyFullPathCMD($$1,$$2)
    QMAKE_POST_LINK += $${CMD}
    export(QMAKE_POST_LINK)
}




