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

    //必须含有canip
    void setNodeInfo(const QList<GNodeInfo>& info);
    const QList<GNodeInfo>& getNodeInfo() const;

    //必须和setNodeInfo的list一致
    void updateValue(const QList<GNodeInfo>& value);
    void updateValue(int canip, const QList<GNodeInfo>& value);
    void update();
    void clear();
    void setCanIpFieldID(const QString& field);
    void setCanIps(const QList<int>& canips);

private:
    QList<GNodeInfo> mData;
    GTable mTable;
    QString mCanipfield;
    QList<int> mCanips;
};

#endif // GIDUTABLEMODEL_H
