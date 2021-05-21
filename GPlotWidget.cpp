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

GPlotWidget::GPlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlotWidget),
    mTemplate(nullptr)
{
    ui->setupUi(this);

    m_treeModel = new QStandardItemModel(this);
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
    connect(ui->treeView,&QTreeView::doubleClicked,this,&GPlotWidget::onTreeViewDoubleClicked);
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
    mTemplate = temp;
    resetTreeView();
}


void GPlotWidget::resetSplitterRatio()
{
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 6);
}

SACustomPlot *GPlotWidget::figure()
{
    return ui->figure;
}


void GPlotWidget::resetTreeView()
{
    if (!mTemplate) {
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
        infos = mTemplate->getSystemInfoList();
        srcTableName = "system";
        replaceSrcTableName = QStringLiteral("系统");
        break;

    case ITEM_DATA_TYPE_Module:
        tables = m_data.getModuleTables();
        infos = mTemplate->getModuleInfoList();
        srcTableName = "module";
        replaceSrcTableName = QStringLiteral("模块");
        break;

    case ITEM_DATA_TYPE_Idu:
        tables = m_data.getIduTables();
        infos = mTemplate->getIduInfoList();
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
                baseItem->appendRow(item);
            }
            //
            m_treeModel->appendRow(baseItem);
        }
    }
}

QVector<QCPGraphData> GPlotWidget::toPlotData(GHvacDataInfo::SeriesPtr x, GHvacDataInfo::SeriesPtr y)
{
    int len = qMin(x->size(),y->size());
    QVector<QCPGraphData> data(len);
    for(int i=0;i<len;++i){
        data[i].key = QDateTime::fromMSecsSinceEpoch( x->at(i) ).toTime_t();
        data[i].value = y->at(i);
    }
    return data;
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
        if(y == nullptr || x == nullptr){
            if(y == nullptr){
                qDebug() << QStringLiteral("y为空,src=%1").arg(ds);
            }else{
                qDebug() << QStringLiteral("x为空,src=%1").arg(m_data.fieldOfDatetime);
            }
            return;
        }
        QCPGraph* g = ui->figure->addGraph();
        g->setName(item->text());
        g->data()->set(toPlotData(x,y));
        g->setPen(m_colormap.nextColor());
        ui->figure->rescaleAxes();
        m_ptrToGraph[y] = g;
        qDebug() << QStringLiteral("绘图:")+item->text();
    }else{
        QCPGraph* g = m_ptrToGraph.value(y,nullptr);
        if(g){
            ui->figure->removeGraph(g);
            ui->figure->rescaleAxes();
            qDebug() << QStringLiteral("移除绘图:")+item->text();
        }
    }
    ui->figure->replot(QCustomPlot::rpQueuedReplot);
}

/**
 * @brief 双击效果
 * @param index
 */
void GPlotWidget::onTreeViewDoubleClicked(const QModelIndex &index)
{
    QStandardItem* item = m_treeModel->itemFromIndex(index);
    if(item){
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
