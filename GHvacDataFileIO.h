#ifndef GHVACDATAFILEIO_H
#define GHVACDATAFILEIO_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QFile>
#include "quazip.h"
#include <QDir>
#include "SATable.h"
#include "GHvacDataInfo.h"
class GHvacDataFileIO : public QObject
{
    Q_OBJECT
public:
    enum Error {
        UnknowError		= 0
        , BadDataFileError	= ZIP_BADZIPFILE        ///< 错误的文件
        , ParamError		= ZIP_PARAMERROR        ///< 参数异常
        , InternalError		= ZIP_INTERNALERROR
    };
    typedef SARowTable<double>	TableType;
    typedef TableType::TablePtr	TablePtr;
    GHvacDataFileIO(QObject *p = nullptr);
    ~GHvacDataFileIO();
    void setFileName(const QString& filepath);
    Error getError() const;
    void setDatetimeField(const QString& f);
    void setCanipField(const QString& canipfield);

public slots:
    void open(const QString& filepath);

signals:

    /**
     * @brief 发生错误
     * @param err
     */
    void error(const QString& err);

    /**
     * @brief 读取完表格发送
     */
    void readed(GHvacDataInfo info);

    /**
     * @brief message
     * @param msg
     */
    void message(const QString& msg);

private:
    void converStringListToDoubleList(const QStringList& str, QVector<double>& d);

    //根据can ip进行groupby
    void groupByCanIP(QList<TablePtr> tables);

    //对时间进行对齐
    void unionDateTime();

    //对时间排序
    void orderByDatetime();

private:
    QString mFileName;
    std::shared_ptr<QuaZip> mZip;
    bool mIsOpen;
    GHvacDataInfo mHvacInfo;
    QString mCanipfield;
};
Q_DECLARE_METATYPE(QList<GHvacDataFileIO::TablePtr>)
Q_DECLARE_METATYPE(QList<int>)
#endif // GHVACDATAFILEIO_H
