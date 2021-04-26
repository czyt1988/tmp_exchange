#include "GHvacDataInfo.h"
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


QJsonObject GHvacDataInfo::get(int toSecsSinceEpoch)
{
    QJsonObject sysobj;
    QJsonObject moduleobj;
    QJsonObject iduobj;

    for (TablePtr t : tables)
    {
        QPair<TableType::Type, int> f = t->lowerBound(toSecsSinceEpoch, fieldOfDatetime);
        SeriesPtr column = t->colunm(f.second);
        QStringList header = t->rowNames();
        int size = std::min(column->size(), header.size());
        if (t->getName().contains("system")) {
            for (int i = 0; i < size; ++i)
            {
                sysobj.insert(header[i], QString::number(column->at(i)));
            }
        }else if (t->getName().contains("module")) {
            QJsonObject d;
            for (int i = 0; i < size; ++i)
            {
                d.insert(header[i], QString::number(column->at(i)));
            }
            moduleobj.insert(t->getName(), d);
        }else if (t->getName().contains("idu")) {
            QJsonObject d;
            for (int i = 0; i < size; ++i)
            {
                d.insert(header[i], QString::number(column->at(i)));
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