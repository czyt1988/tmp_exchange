#ifndef GIDUTABLEMODEL_H
#define GIDUTABLEMODEL_H

#include <QtCore/qglobal.h>
#include <QAbstractTableModel>
#include <QMap>
#include "GNodeInfo.h"
#include "SATable.h"
class GIDUTableModel : public QAbstractTableModel
{
public:
    GIDUTableModel(QObject *par = nullptr);
    typedef SARowTable<QVariant>			GTable;
    typedef SARowTable<QVariant>::SeriesType	TableRow;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    void setNodeInfo(const QList<GNodeInfo>& info);
    void update();
    void clear();

private:
    QList<GNodeInfo> mData;
    GTable mTable;
};

#endif // GIDUTABLEMODEL_H
