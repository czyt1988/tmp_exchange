#ifndef GHVACDATAFILEIO_H
#define GHVACDATAFILEIO_H

#include <QtCore/qglobal.h>
#include <QFile>
#include "quazip.h"
#include <QDir>
#include "SATable.h"
class GHvacDataFileIO
{
public:
    enum Error {
        UnknowError		= 0
        , BadDataFileError	= ZIP_BADZIPFILE        ///< 错误的文件
        , ParamError		= ZIP_PARAMERROR        ///< 参数异常
        , InternalError		= ZIP_INTERNALERROR
    };
    GHvacDataFileIO();
    GHvacDataFileIO(const QString& filepath);
    void setFileName(const QString& filepath);
    bool open();
    Error getError() const;

private:
    void converStringListToDoubleList(const QStringList& str, QVector<double>& d);

private:
    QuaZip mZip;
    bool mIsOpen;
    QStringList mFilesList;
    QDir mTmpDir;
    typedef SARowTable<double>		TableType;
    typedef std::shared_ptr<TableType>	TablePtr;
    TablePtr mSys;
    TablePtr mModule;
    TablePtr mIdu;
};

#endif // GHVACDATAFILEIO_H
