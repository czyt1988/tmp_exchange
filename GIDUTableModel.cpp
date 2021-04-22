#include "GIDUTableModel.h"

GIDUTableModel::GIDUTableModel(QObject *par) : QAbstractTableModel(par)
{
}


QVariant GIDUTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section < mData.size()) {
                return (mData[section].mName);
            }
        }else{
            return (QString::number(section+1));
        }
    }
    return (QVariant());
}


int GIDUTableModel::columnCount(const QModelIndex& parent) const
{
    return (mData.size());
}


int GIDUTableModel::rowCount(const QModelIndex& parent) const
{
    return (mTable.rowCount());
}


QVariant GIDUTableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        return (mTable.cell(index.row(), index.column()));
    }
    return (QVariant());
}


void GIDUTableModel::setNodeInfo(const QList<GNodeInfo>& info)
{
    beginResetModel();
    mData = info;
    endResetModel();
}


void GIDUTableModel::update()
{
    beginResetModel();
    endResetModel();
}


void GIDUTableModel::clear()
{
    beginResetModel();
    mData.clear();
    mTable.clear();
    endResetModel();
}
