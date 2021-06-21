#include "SACustomPlotTreeModel.h"
#if USE_STANDARD_ITEM_MODEL

#define ROLE_LEVEL		Qt::UserRole + 1        //用于标记一些节点,所有节点包括参数都需要有

#define LEVEL_NAME		0                       // 其他名字条目
#define LEVEL_VALUE		1                       // 其他参数条目
#define LEVEL_BACKGROUND                                //背景
class SACustomPlotTreeModelPrivate {
    SA_IMPL_PUBLIC(SACustomPlotTreeModel)
public:
    SACustomPlotTreeModelPrivate(SACustomPlotTreeModel *p) : q_ptr(p)
        , mPlot(nullptr)
    {
    }


    QPointer<QCustomPlot> mPlot;
};

SACustomPlotTreeModel::SACustomPlotTreeModel(QObject *p) : QStandardItemModel(p)
    , d_ptr(new SACustomPlotTreeModelPrivate(this))
{
}


void SACustomPlotTreeModel::setPlot(QCustomPlot *plot)
{
    if (plot == d_ptr->mPlot) {
        update();
        return;
    }
    reset();
    d_ptr->mPlot = plot;
    update();
}


void SACustomPlotTreeModel::update()
{
    //开始更新
    int rc = rowCount();

    for (int i = 0; i < 5; ++i)
    {
        if (i < rc) {
            QStandardItem *rootitem = item(i);
        }
    }
}


void SACustomPlotTreeModel::reset()
{
    clear();
    setHorizontalHeaderLabels({ tr("name"), tr("value") });
}


#else
#include <QPointer>
class SACustomPlotTreeModelPrivate {
    SA_IMPL_PUBLIC(SACustomPlotTreeModel)
public:
    SACustomPlotTreeModelPrivate(SACustomPlotTreeModel *p) : q_ptr(p)
        , mPlot(nullptr)
    {
    }


    QPointer<QCustomPlot> mPlot;
};


SACustomPlotTreeModel::SACustomPlotTreeModel(QObject *p) : QAbstractItemModel(p)
    , d_ptr(new SACustomPlotTreeModelPrivate(this))
{
}


QModelIndex SACustomPlotTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if ((row < 0) || (column < 0) || (d_ptr->mPlot == nullptr)) {
        return (QModelIndex());
    }
    //说明是顶层 顶层对应chart
    if (!parent.isValid()) {
        if (row >= 4) {
            return (QModelIndex());
        }
        return (createIndex(row, 0, d_ptr->mPlot.data()));//顶层节点有4个
    }
    //不是顶层
    void *ppoint = parent.internalPointer();

    if (ppoint == d_ptr->mPlot) {
        //说明是第二层
        QCustomPlot *plot = d_ptr->mPlot.data();
        int fr = parent.row();
        if (0 == fr) {
            //背景没有下一级
            return (QModelIndex());
        }else if (1 == fr) {
            //graph 层
            if ((row < plot->graphCount()) && (0 == column)) {
                return (createIndex(row, 0, plot->graph(row)));
            }
        }else if (2 == fr) {
            //layer 层
            if ((row < plot->layerCount()) && (0 == column)) {
                return (createIndex(row, 0, plot->layer(row)));
            }
        }else if (3 == fr) {
            //axis 层
            if ((row < plot->axisRectCount()) && (0 == column)) {
                return (createIndex(row, 0, plot->axisRect(row)));
            }
        }
    }
    return (QModelIndex());
}


QModelIndex SACustomPlotTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return (QModelIndex());
    }
    void *p = index.internalPointer();

    if (p == d_ptr->mPlot) {
        //顶层节点返回空
        return (QModelIndex());
    }
    //看看是否是graph
    int r = 0;

    r = d_ptr->mPlot->graphCount();

    for (int i = 0; i < r; ++i)
    {
        if (p == d_ptr->mPlot->graph(i)) {
            return (createIndex(1, 0, d_ptr->mPlot.data()));
        }
    }

    //看看是否是layer
    r = d_ptr->mPlot->layerCount();
    for (int i = 0; i < r; ++i)
    {
        if (p == d_ptr->mPlot->layer(i)) {
            return (createIndex(2, 0, d_ptr->mPlot.data()));
        }
    }

    //看看是否是axis
    r = d_ptr->mPlot->axisRectCount();
    for (int i = 0; i < r; ++i)
    {
        if (p == d_ptr->mPlot->axisRect(i)) {
            return (createIndex(3, 0, d_ptr->mPlot.data()));
        }
    }
    return (QModelIndex());
}


int SACustomPlotTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {//说明是顶层
        return (4);
    }
    //说明非顶层
    void *p = parent.internalPointer();

    if (p == d_ptr->mPlot) {
        //第一层
        switch (parent.row())
        {
        case 0:
            return (1);

        case 1:
            return (d_ptr->mPlot->graphCount());

        case 2:
            return (d_ptr->mPlot->layerCount());

        case 3:
            return (d_ptr->mPlot->axisRectCount());

        default:
            break;
        }
    }
    return (0);
}


int SACustomPlotTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return (2);
}


QVariant SACustomPlotTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return (QVariant());
    }
    if (Qt::Horizontal == orientation) {//说明是水平表头
        switch (section)
        {
        case 0:
            return (tr("name"));

        case 1:
            return (tr("value"));//值

        default:
            break;
        }
        return (QString());
    }
    return (QVariant());
}


Qt::ItemFlags SACustomPlotTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return (Qt::NoItemFlags);
    }
    void *p = index.internalPointer();

    if (p == d_ptr->mPlot) {
        //首层
        if (0 == index.row()) {//_background
            if (1 == index.column()) {
                return (Qt::ItemIsEditable | Qt::ItemIsSelectable |Qt::ItemIsEnabled);
            }
        }
        return (Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }
    return (Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}


QVariant SACustomPlotTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return (QVariant());
    }
    switch (role)
    {
    case Qt::DisplayRole:
        return (dataDisplayRole(index));

    case Qt::BackgroundRole:
        return (dataBackgroundRole(index));

    case Qt::DecorationRole:
        return (dataDecorationRole(index));

    default:
        break;
    }
    return (QVariant());
}


void SACustomPlotTreeModel::setPlot(QCustomPlot *plot)
{
    beginResetModel();
    d_ptr->mPlot = plot;
    endResetModel();
}


void SACustomPlotTreeModel::update()
{
    beginResetModel();
    endResetModel();
}


QVariant SACustomPlotTreeModel::dataDisplayRole(const QModelIndex& index) const
{
    void *p = index.internalPointer();

    if (p == d_ptr->mPlot) {
        if (index.column() == 0) {
            switch (index.row())
            {
            case 0:
                return (tr("background"));

            case 1:
                return (tr("graph"));

            case 2:
                return (tr("layer"));

            case 3:
                return (tr("axis"));

            default:
                break;
            }
        }
        return (QVariant());
    }else {
        //看看是否是graph
        int r = 0;
        r = d_ptr->mPlot->graphCount();

        for (int i = 0; i < r; ++i)
        {
            if (p == d_ptr->mPlot->graph(i)) {
                if (0 == index.column()) {
                    return (d_ptr->mPlot->graph(i)->name());
                }
            }
        }
        r = d_ptr->mPlot->layerCount();

        for (int i = 0; i < r; ++i)
        {
            if (p == d_ptr->mPlot->layer(i)) {
                QCPLayer *lay = d_ptr->mPlot->layer(i);
                if (0 == index.column()) {
                    return (lay->name());
                }
            }
        }

        r = d_ptr->mPlot->axisRectCount();

        for (int i = 0; i < r; ++i)
        {
            if (p == d_ptr->mPlot->axisRect(i)) {
                QCPAxisRect *axis = d_ptr->mPlot->axisRect(i);
                if (0 == index.column()) {
                    return (tr("axis %1").arg(i+1));
                }
            }
        }
    }
    return (QVariant());
}


QVariant SACustomPlotTreeModel::dataBackgroundRole(const QModelIndex& index) const
{
    return (QVariant());
}


QVariant SACustomPlotTreeModel::dataDecorationRole(const QModelIndex& index) const
{
    return (QVariant());
}


#endif
