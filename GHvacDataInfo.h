#ifndef GHVACDATAINFO_H
#define GHVACDATAINFO_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QDateTime>
#include <QList>
#include "SATable.h"
#include <QJsonObject>

/**
 * @brief 保存读取后的结果，并通过@sa get 函数获取某一个时刻的json
 */
class GHvacDataInfo
{
public:
    typedef double			NumType;
    typedef SARowTable<NumType>	TableType;
    typedef TableType::TablePtr	TablePtr;
    typedef TableType::SeriesPtr	SeriesPtr;

public:
    GHvacDataInfo();
    void clear();
    QJsonObject get(int toSecsSinceEpoch);

    //获取系统表
    QList<TablePtr> getSystemTables() const;

    //获取模块表
    QList<TablePtr> getModuleTables() const;

    //获取内机表
    QList<TablePtr> getIduTables() const;

public:
    QList<int> moduleCanIPs;
    QList<int> iduCanIPs;
    QList<QDateTime> allDateTimeScale;
    QStringList filesList;
    QList<TablePtr> tables;
    QString fieldOfDatetime;///< 时间字段
};
Q_DECLARE_METATYPE(GHvacDataInfo)
#endif // GHVACDATAINFO_H
