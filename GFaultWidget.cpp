#include "GFaultWidget.h"
#include "ui_GFaultWidget.h"
#include <algorithm>
#include <QDebug>
#define DATA_DATETIME    (Qt::UserRole + 1)
GFaultWidget::GFaultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GFaultWidget)
    , mTemplate(nullptr)
{
    ui->setupUi(this);
    m_model = new QStandardItemModel(this);
    ui->tableView->setModel(m_model);
}


GFaultWidget::~GFaultWidget()
{
    delete ui;
}


void GFaultWidget::ininModel()
{
    m_model->setHorizontalHeaderLabels({ QStringLiteral("位置"), QStringLiteral("故障名"), QStringLiteral("故障发生时间") });
}


void GFaultWidget::setTemplate(GTemplate *temp)
{
    mTemplate = temp;
}


void GFaultWidget::setData(const GHvacDataInfo& res)
{
    m_resInfo = res;
}


/**
 * @brief 更新故障信息，此函数只需调用一次
 * @param sys
 * @param module
 * @param idu
 * @param res
 */
void GFaultWidget::updateFaultInfo()
{
    //先清除model
    m_model->clear();
    ininModel();
    if (mTemplate == nullptr) {
        return;
    }
    //查询系统故障
    GHvacDataInfo::TablePtr tp = nullptr;

    const QList<GNodeInfo>& systemFaultSrc = mTemplate->getSystemFaultInfoList();
    const QList<GNodeInfo>& moduleFaultSrc = mTemplate->getModuleFaultInfoList();
    const QList<GNodeInfo>& iduFaultSrc = mTemplate->getIduFaultInfoList();

    //查找系统故障
    for (int i = 0; i < m_resInfo.tables.size(); ++i)
    {
        if (m_resInfo.tables[i]->getName().contains("system", Qt::CaseInsensitive)) {
            tp = m_resInfo.tables[i];
            for (const GNodeInfo& info : systemFaultSrc)
            {
                findFault(info, tp, QIcon(":/icon/icon/system-fault.svg"));
            }
        }
    }
    //查找模块故障
    for (int i = 0; i < m_resInfo.tables.size(); ++i)
    {
        if (m_resInfo.tables[i]->getName().contains("module", Qt::CaseInsensitive)) {
            tp = m_resInfo.tables[i];
            for (const GNodeInfo& info : moduleFaultSrc)
            {
                findFault(info, tp, QIcon(":/icon/icon/module-fault.svg"));
            }
        }
    }
    //查找内机故障
    for (int i = 0; i < m_resInfo.tables.size(); ++i)
    {
        if (m_resInfo.tables[i]->getName().contains("idu", Qt::CaseInsensitive)) {
            tp = m_resInfo.tables[i];
            for (const GNodeInfo& info : iduFaultSrc)
            {
                findFault(info, tp, QIcon(":/icon/icon/idu-fault.svg"));
            }
        }
    }
}


int GFaultWidget::findFault(const GNodeInfo& info, GHvacDataInfo::TablePtr tp, const QIcon& icon)
{
    const GHvacDataInfo::SeriesPtr ser = tp->row(info.mSrc);

    if (ser == nullptr) {
        qDebug() << QStringLiteral("数据表无：") << info.mSrc;
        return (0);
    }
    auto fr = std::find_if(ser->begin(), ser->end(), [](GHvacDataInfo::NumType i)->bool {
        return (qFuzzyCompare(i, 1.0));
    });

    if (fr == ser->end()) {
        return (0);
    }
    //说明找到，生成item
    int dcol = std::distance(ser->begin(), fr);
    int mr = m_model->rowCount();
    QStandardItem *itemLoc = nullptr;
    QStandardItem *itemName = nullptr;
    QStandardItem *itemDatetime = nullptr;

    //位置
    if (icon.isNull()) {
        itemLoc = new QStandardItem(tp->getName());
        m_model->setItem(mr, 0, itemLoc);
    }else{
        itemLoc = new QStandardItem(icon, tp->getName());
        m_model->setItem(mr, 0, itemLoc);
    }
    //故障名
    itemName = new QStandardItem(info.mName);
    m_model->setItem(mr, 1, itemName);
    //故障时间
    const GHvacDataInfo::SeriesPtr dateser = tp->row(m_resInfo.fieldOfDatetime);

    if (dateser == nullptr) {
        return (1);
    }
    if (dcol >= dateser->size()) {
        qDebug()	<< dateser->getName() << QStringLiteral(" -> 获取长度越界：") << dcol
                << QStringLiteral(",序列长度为：")<< dateser->size();
        return (1);
    }
    QDateTime d = QDateTime::fromSecsSinceEpoch(dateser->at(dcol));

    itemDatetime = new QStandardItem(d.toString("yyyy-MM-dd HH:mm:ss"));
    m_model->setItem(mr, 2, itemDatetime);
    itemDatetime->setData(d, DATA_DATETIME);
    itemLoc->setData(d, DATA_DATETIME);
    itemName->setData(d, DATA_DATETIME);
    return (1);
}


void GFaultWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

    default:
        break;
    }
}


/**
 * @brief item双击跳转到对应的序号中
 * @param index
 */
void GFaultWidget::on_tableView_doubleClicked(const QModelIndex& index)
{
    QStandardItem *item = m_model->itemFromIndex(index);

    if (nullptr == item) {
        return;
    }
    QVariant d = item->data(DATA_DATETIME);

    if (!d.isValid()) {
        return;
    }
    QDateTime datetime = d.toDateTime();

    if (!datetime.isValid()) {
        return;
    }
    int i = m_resInfo.allDateTimeScale.indexOf(datetime);
    emit indexReques(i);
}
