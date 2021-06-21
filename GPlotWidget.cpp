#include "GPlotWidget.h"
#include "ui_GPlotWidget.h"
#include "SATable.h"
#include <QDebug>
#include "SACustomPlot.h"
#include <QDebug>
#include <QDateTime>
#define ROLE_ITEM_DATA_HVACTYPE		(Qt::UserRole+1)
#define ITEM_DATA_TYPE_System		1
#define ITEM_DATA_TYPE_Module		2
#define ITEM_DATA_TYPE_Idu		3
//数据字段
#define ROLE_ITEM_DATA_SRC		(Qt::UserRole+2)
//数据表指针
#define ROLE_ITEM_DATA_TABLEPTR		(Qt::UserRole+3)
//item 的id为了搜索model和主model同步用
#define ROLE_ITEM_ID			(Qt::UserRole+4)

GPlotWidget::GPlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlotWidget),
    m_template(nullptr)
{
    ui->setupUi(this);

    m_treeModel = new QStandardItemModel(this);
    m_searchModel = new QStandardItemModel(this);
    ui->treeView->setModel(m_treeModel);
    //添加一个默认绘图
    resetSplitterRatio();
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);

    dateTicker->setDateTimeFormat("yyyy-MM-dd\nHH:mm:ss");
    ui->figure->xAxis->setTicker(dateTicker);
    ui->figure->axisRect()->setupFullAxesBox();
    ui->figure->legend->setVisible(true);
    ui->figure->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_colormap.setColorMap(SAColorMap::Tab10);

    connect(m_treeModel, &QStandardItemModel::itemChanged, this, &GPlotWidget::onModelItemChanged);
    connect(m_searchModel, &QStandardItemModel::itemChanged, this, &GPlotWidget::onSearchModelItemChanged);
    connect(ui->treeView, &QTreeView::doubleClicked, this, &GPlotWidget::onTreeViewDoubleClicked);
    // connect signals so top and right axes move in sync with bottom and left axes:
    connect(ui->figure->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->figure->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->figure->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->figure->yAxis2, SLOT(setRange(QCPRange)));
}


GPlotWidget::~GPlotWidget()
{
    delete ui;
}


void GPlotWidget::setData(const GHvacDataInfo& d)
{
    m_data = d;
    resetTreeView();
    ui->treeView->expandAll();
}


void GPlotWidget::setTemplate(GTemplate *temp)
{
    m_template = temp;
    resetTreeView();
}


void GPlotWidget::resetSplitterRatio()
{
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 6);
}


SACustomPlot *GPlotWidget::figure()
{
    return (ui->figure);
}


void GPlotWidget::resetTreeView()
{
    if (!m_template) {
        return;
    }
    m_treeModel->clear();
    //系统

    makeTree(ITEM_DATA_TYPE_System);
    makeTree(ITEM_DATA_TYPE_Module);
    makeTree(ITEM_DATA_TYPE_Idu);
    ui->treeView->expandAll();
}


void GPlotWidget::makeTree(int itemDataType)
{
    QList<GHvacDataInfo::TablePtr> tables;
    QString srcTableName = "";
    QString replaceSrcTableName = "";
    QList<GNodeInfo> infos;

    switch (itemDataType)
    {
    case ITEM_DATA_TYPE_System:
        tables = m_data.getSystemTables();
        infos = m_template->getSystemInfoList();
        srcTableName = "system";
        replaceSrcTableName = QStringLiteral("系统");
        break;

    case ITEM_DATA_TYPE_Module:
        tables = m_data.getModuleTables();
        infos = m_template->getModuleInfoList();
        srcTableName = "module";
        replaceSrcTableName = QStringLiteral("模块");
        break;

    case ITEM_DATA_TYPE_Idu:
        tables = m_data.getIduTables();
        infos = m_template->getIduInfoList();
        srcTableName = "idu";
        replaceSrcTableName = QStringLiteral("内机");
        break;

    default:
        return;
    }


    if (!tables.isEmpty()) {
        for (GHvacDataInfo::TablePtr t : tables)
        {
            QString tname = t->getName();
            tname = tname.replace(srcTableName, replaceSrcTableName);
            QStandardItem *baseItem = new QStandardItem(tname);
            baseItem->setData(itemDataType, ROLE_ITEM_DATA_HVACTYPE);
            //获取系统表下所有参数字段
            QStringList ns = t->rowNames();
            for (const QString& n : ns)
            {
                QString displayname = findNameBySrc(infos, n);
                if (n.isEmpty() || displayname.isEmpty()) {
                    continue;
                }
                QStandardItem *item = new QStandardItem(displayname);
                item->setCheckable(true);
                item->setData(itemDataType, ROLE_ITEM_DATA_HVACTYPE);
                item->setData(n, ROLE_ITEM_DATA_SRC);
                item->setData(QVariant::fromValue((quintptr)t.get()), ROLE_ITEM_DATA_TABLEPTR);
                item->setData(qulonglong(item), ROLE_ITEM_ID);
                baseItem->appendRow(item);
            }
            //
            baseItem->setData(qulonglong(baseItem), ROLE_ITEM_ID);
            m_treeModel->appendRow(baseItem);
        }
    }
}


QVector<QCPGraphData> GPlotWidget::toPlotData(GHvacDataInfo::SeriesPtr x, GHvacDataInfo::SeriesPtr y)
{
    int len = qMin(x->size(), y->size());
    QVector<QCPGraphData> data(len);

    for (int i = 0; i < len; ++i)
    {
        data[i].key = QDateTime::fromMSecsSinceEpoch(x->at(i)).toTime_t();
        data[i].value = y->at(i);
    }
    return (data);
}


/**
 * @brief 查找
 * @param keyword
 */
void GPlotWidget::search(const QString& keyword)
{
    QStringList kws = keyword.split(" ");

    if (keyword.isEmpty()) {
        ui->treeView->setModel(m_treeModel);
    }else{
        m_searchModel->clear();
        //遍历，如果没有ROLE_ITEM_DATA_SRC角色的是都要保留
        int br = m_treeModel->rowCount();
        for (int i = 0; i < br; ++i)
        {
            QStandardItem *orgRootItem = m_treeModel->item(i);
            QStandardItem *serRootItem = orgRootItem->clone();
            QMap<QStandardItem *, int> res;
            for (int j = 0; j < orgRootItem->rowCount(); ++j)
            {
                for (const QString& k : kws)
                {
                    QStandardItem *childitem = orgRootItem->child(j);
                    if (childitem->text().contains(k)) {
                        if (res.contains(childitem)) {
                            ++res[childitem];
                        }else{
                            res[childitem] = 1;
                        }
                    }
                }
            }
            if (res.size() > 0) {
                QList<QPair<QStandardItem *, int> > sartitemlist;
                for (auto i = res.begin(); i != res.end(); ++i)
                {
                    sartitemlist.append(qMakePair(i.key(), i.value()));
                }
                std::sort(sartitemlist.begin(), sartitemlist.end(), [](const QPair<QStandardItem *, int>& it1, const QPair<QStandardItem *, int>& it2)->bool {
                    return (it1.second < it2.second);
                });
                for (QPair<QStandardItem *, int> v : sartitemlist)
                {
                    serRootItem->appendRow(v.first->clone());
                }
                m_searchModel->appendRow(serRootItem);
            }else{
                delete serRootItem;
            }
        }
        ui->treeView->setModel(m_searchModel);
        ui->treeView->expandAll();
    }
}


/**
 * @brief 清除某个model的所有选择状态
 * @param model
 */
void GPlotWidget::clearAllState(QStandardItemModel *model)
{
    int r = model->rowCount();

    for (int i = 0; i < r; ++i)
    {
        clearStandardItemState(model->item(i));
    }
}


void GPlotWidget::clearStandardItemState(QStandardItem *item)
{
    int r = item->rowCount();

    for (int i = 0; i < r; ++i)
    {
        clearStandardItemState(item->child(i));
    }
    if (Qt::Unchecked != item->checkState()) {
        item->setCheckState(Qt::Unchecked);
    }
}


QStandardItem *GPlotWidget::findByID(qulonglong id)
{
    int r = m_treeModel->rowCount();

    for (int i = 0; i < r; ++i)
    {
        QStandardItem *item = findByID(m_treeModel->item(i), id);
        if (item) {
            return (item);
        }
    }
    return (nullptr);
}


QStandardItem *GPlotWidget::findByID(QStandardItem *par, qulonglong id)
{
    int r = par->rowCount();

    for (int i = 0; i < r; ++i)
    {
        QStandardItem *item = findByID(par->child(i), id);
        if (item) {
            return (item);
        }
    }
    qulonglong o = par->data(ROLE_ITEM_ID).toULongLong();

    if (o == id) {
        return (par);
    }
    return (nullptr);
}


/**
 * @brief 触发事件，进行绘图
 * @param item
 */
void GPlotWidget::onModelItemChanged(QStandardItem *item)
{
    QVariant vp = item->data(ROLE_ITEM_DATA_TABLEPTR);
    QVariant vds = item->data(ROLE_ITEM_DATA_SRC);

    if (!vp.isValid() || !vds.isValid()) {
        //点击的不是数据
        qDebug() << QStringLiteral("点击的不是数据");
        return;
    }
    GHvacDataInfo::TableType *tableRawPtr = reinterpret_cast<GHvacDataInfo::TableType *>(vp.value<quintptr>());
    QString ds = vds.toString();
    GHvacDataInfo::SeriesPtr y = tableRawPtr->row(ds);

    if (Qt::Checked == item->checkState()) {
        //选中了
        GHvacDataInfo::SeriesPtr x = tableRawPtr->row(m_data.fieldOfDatetime);
        if ((y == nullptr) || (x == nullptr)) {
            if (y == nullptr) {
                qDebug() << QStringLiteral("y为空,src=%1").arg(ds);
            }else{
                qDebug() << QStringLiteral("x为空,src=%1").arg(m_data.fieldOfDatetime);
            }
            return;
        }
        QCPGraph *g = ui->figure->addGraph();
        g->setName(item->text());
        g->data()->set(toPlotData(x, y));
        g->setPen(m_colormap.nextColor());
        ui->figure->rescaleAxes();
        m_ptrToGraph[y] = g;
        qDebug() << QStringLiteral("绘图:")+item->text();
    }else{
        QCPGraph *g = m_ptrToGraph.value(y, nullptr);
        if (g) {
            ui->figure->removeGraph(g);
            ui->figure->rescaleAxes();
            qDebug() << QStringLiteral("移除绘图:")+item->text();
        }
    }
    ui->figure->replot(QCustomPlot::rpQueuedReplot);
}


void GPlotWidget::onSearchModelItemChanged(QStandardItem *item)
{
    QStandardItem *oi = findByID(item->data(ROLE_ITEM_ID).toInt());

    if (oi) {
        oi->setCheckState(item->checkState());
    }
}


/**
 * @brief 双击效果
 * @param index
 */
void GPlotWidget::onTreeViewDoubleClicked(const QModelIndex& index)
{
    QStandardItem *item = m_treeModel->itemFromIndex(index);

    if (nullptr == item) {
        item = m_searchModel->itemFromIndex(index);
    }
    if (item) {
        if (Qt::Checked != item->checkState()) {
            item->setCheckState(Qt::Checked);
        }
    }
}


QString GPlotWidget::findNameBySrc(const QList<GNodeInfo>& infos, const QString& src)
{
    for (const GNodeInfo& i : infos)
    {
        if (i.mSrc == src) {
            return (i.mName);
        }
    }
    QStringList ss;

    for (auto m : infos)
    {
        ss << m.mSrc;
    }
    qDebug()	<< QStringLiteral("无法找到:") << src
            <<"    " << ss;
    return (QString());
}


void GPlotWidget::on_pushButtonSearch_clicked()
{
    search(ui->lineEditSearch->text());
}


void GPlotWidget::on_lineEditSearch_textChanged(const QString& arg1)
{
    search(arg1);
}
