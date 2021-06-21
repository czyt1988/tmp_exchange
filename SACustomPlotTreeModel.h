#ifndef SACUSTOMPLOTTREEMODEL_H
#define SACUSTOMPLOTTREEMODEL_H

#include <QtCore/qglobal.h>
#include "qcustomplot.h"
#include "SAGlobals.h"
SA_IMPL_FORWARD_DECL(SACustomPlotTreeModel)

#define USE_STANDARD_ITEM_MODEL    1

/**
 * @brief 针对qcustomplot的设置数
 *
 * plot
 *  |_background
 *  |_graph
 *  |_layer
 *  |_axisRect
 */

#if USE_STANDARD_ITEM_MODEL

#include <QStandardItemModel>

class SACustomPlotTreeModel : public QStandardItemModel
{
    SA_IMPL(SACustomPlotTreeModel)
public:
    SACustomPlotTreeModel(QObject *p = nullptr);
    void setPlot(QCustomPlot *plot);
    void update();

private:
    void reset();
};
#else
#include <QAbstractItemModel>
class SACustomPlotTreeModel : public QAbstractItemModel
{
    SA_IMPL(SACustomPlotTreeModel)
public:
    SACustomPlotTreeModel(QObject *p = nullptr);
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    void setPlot(QCustomPlot *plot);
    void update();

private:
    QVariant dataDisplayRole(const QModelIndex& index) const;
    QVariant dataBackgroundRole(const QModelIndex& index) const;
    QVariant dataDecorationRole(const QModelIndex& index) const;
};
#endif
#endif // QCUSTOMPLOTTREEMODEL_H
