﻿#include "GHvacDataInfo.h"
#include <QDebug>
#include <QJsonDocument>
GHvacDataInfo::GHvacDataInfo()
{
}


void GHvacDataInfo::clear()
{
    moduleCanIPs.clear();
    iduCanIPs.clear();
    allDateTimeScale.clear();
    filesList.clear();
    tables.clear();
}


QJsonObject GHvacDataInfo::get(qint64 msecsSinceEpoch)
{
    QJsonObject sysobj;
    QJsonObject moduleobj;
    QJsonObject iduobj;

    for (TablePtr t : tables)
    {
        QPair<TableType::Type, int> f = t->lowerBound(msecsSinceEpoch, this->fieldOfDatetime);

//        qDebug() << "  table:" << t->getName() << " searched index:" << f.second
//                 << " date range is[" << t->row(this->fieldOfDatetime)->first() << ","
//                 <<t->row(this->fieldOfDatetime)->back() <<"]";

        SeriesPtr column = t->colunm(f.second);
        QStringList header = t->rowNames();
        int size = std::min(column->size(), header.size());
        if (t->getName().contains("system")) {
            for (int i = 0; i < size; ++i)
            {
                sysobj.insert(header[i].toLower(), QString::number(column->at(i)));
            }
        }else if (t->getName().contains("module")) {
            QJsonObject d;
            for (int i = 0; i < size; ++i)
            {
                d.insert(header[i].toLower(), QString::number(column->at(i)));
            }
            moduleobj.insert(t->getName(), d);
        }else if (t->getName().contains("idu")) {
            QJsonObject d;
            for (int i = 0; i < size; ++i)
            {
                d.insert(header[i].toLower(), QString::number(column->at(i)));
            }
            iduobj.insert(t->getName(), d);
        }
    }
    QJsonObject rootobj;

    rootobj.insert("system", sysobj);
    rootobj.insert("module", moduleobj);
    rootobj.insert("idu", iduobj);
    return (rootobj);
}


/**
 * @brief 获取系统表
 * @return
 */
QList<GHvacDataInfo::TablePtr> GHvacDataInfo::getSystemTables() const
{
    QList<GHvacDataInfo::TablePtr> res;

    for (TablePtr t : this->tables)
    {
        if (t->getName().contains("system")) {
            res.append(t);
        }
    }
    return (res);
}


/**
 * @brief GHvacDataInfo::getModuleTables
 * @return
 */
QList<GHvacDataInfo::TablePtr> GHvacDataInfo::getModuleTables() const
{
    QList<GHvacDataInfo::TablePtr> res;

    for (TablePtr t : this->tables)
    {
        if (t->getName().contains("module")) {
            res.append(t);
        }
    }
    return (res);
}


/**
 * @brief GHvacDataInfo::getIduTables
 * @return
 */
QList<GHvacDataInfo::TablePtr> GHvacDataInfo::getIduTables() const
{
    QList<GHvacDataInfo::TablePtr> res;

    for (TablePtr t : this->tables)
    {
        if (t->getName().contains("idu")) {
            res.append(t);
        }
    }
    return (res);
}


QDebug operator<<(QDebug debug, const GHvacDataInfo& c)
{
    QDebugStateSaver saver(debug);

    debug	<< "GHvacDataInfo:"
        << "\n  filesList:" << c.filesList
        << "\n  projectid:" << c.projectid
        << "\n  fieldOfDatetime:" << c.fieldOfDatetime
        << "\n  moduleCanIPs:" << c.moduleCanIPs
        << "\n  iduCanIPs:" << c.iduCanIPs
    ;

    return (debug);
}
