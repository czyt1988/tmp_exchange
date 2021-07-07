#include "GIDUTableModel.h"
#include <cmath>
#include <numeric>
GIDUTableModel::GIDUTableModel(QObject *par) : QAbstractTableModel(par)
{
    mTable.setMode(GTable::ExpandMode);
}


QVariant GIDUTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section < mData.size()) {
                return (mData[section].mName);
            }
        }else{
            if (section < mTable.rowCount()) {
                return (mTable.row(section)->getName());
            }
            return (QString::number(section+1));
        }
    }
    return (QVariant());
}


int GIDUTableModel::columnCount(const QModelIndex& parent) const
{
    if (mTable.columnCount() == 0) {
        return (mData.size());
    }
    return (mTable.columnCount());
}


int GIDUTableModel::rowCount(const QModelIndex& parent) const
{
    return (mTable.rowCount());
}


QVariant GIDUTableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        if ((index.row() < mTable.rowCount()) && (index.column() < mTable.columnCount())) {
            return (mTable.cell(index.row(), index.column()));
        }
        return (QVariant());
    }
    return (QVariant());
}


void GIDUTableModel::setNodeInfo(const QList<GNodeInfo>& info)
{
    beginResetModel();
    mData = info;
    if (mCanips.size() > 0) {
        mTable.resize(mCanips.size(), mData.size());
        mTable.fill(NAN);
        for (int i = 0; i < mCanips.size(); ++i)
        {
            mTable.row(i)->setName(QString("%1").arg(mCanips[i]));
        }
    }
    endResetModel();
}


const QList<GNodeInfo>& GIDUTableModel::getNodeInfo() const
{
    return (mData);
}


void GIDUTableModel::updateValue(int canip, const QList<GNodeInfo>& value)
{
    beginResetModel();
    if (value.isEmpty()) {
        qDebug() << QStringLiteral("传入空的list");
        return;
    }
    QString canipstr = QString::number(canip);
    int r = mTable.nameToIndex(canipstr);

    if (r < 0) {
        //插入一个canip
        GTable::SeriesPtr ser = GTable::makeSeries(canipstr);
        ser->resize(mData.size());
        mTable.appendRow(ser);
        r = mTable.nameToIndex(canipstr);
    }
    int s = qMin(value.size(), mTable.columnCount());

    for (int i = 0; i < s; ++i)
    {
        mTable[r][i] = value[i].mDisplayValue;
    }
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


void GIDUTableModel::setCanIpFieldID(const QString& field)
{
    mCanipfield = field;
}


void GIDUTableModel::setCanIps(const QList<int>& canips)
{
    beginResetModel();
    mCanips = canips;
    if (mData.size() > 0) {
        mTable.resize(canips.size(), mData.size());
        mTable.fill(QVariant());
        for (int i = 0; i < canips.size(); ++i)
        {
            mTable.row(i)->setName(QString("%1").arg(canips[i]));
        }
    }
    endResetModel();
}
