#include "GFaultWidget.h"
#include "ui_GFaultWidget.h"
#include <algorithm>
GFaultWidget::GFaultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GFaultWidget)
{
    ui->setupUi(this);
    m_model = new QStandardItemModel(this);
}


GFaultWidget::~GFaultWidget()
{
    delete ui;
}


void GFaultWidget::ininModel()
{
    m_model->setHorizontalHeaderLabels({ QStringLiteral("位置"), QStringLiteral("故障名"), QStringLiteral("故障发生时间") });
}


/**
 * @brief 更新故障信息，此函数只需调用一次
 * @param sys
 * @param module
 * @param idu
 * @param res
 */
void GFaultWidget::updateFaultInfo(const QList<GNodeInfo>& sys, const QList<GNodeInfo>& module, const QList<GNodeInfo>& idu, const GHvacDataInfo& res)
{
    //先清除model
    m_model->clear();
    ininModel();
    //查询系统故障
    GHvacDataInfo::TablePtr tp = nullptr;

    mSystemFaultSrc = sys;
    mModuleFaultSrc = module;
    mIduFaultSrc = idu;
    m_resInfo = res;
    for (int i = 0; i < res.tables.size(); ++i)
    {
        if (res.tables[i]->getName() == "system") {
            tp = res.tables[i];
            break;
        }
    }
    if (tp) {
        for (const GNodeInfo& info : sys)
        {
            const GHvacDataInfo::SeriesPtr ser = tp->row(info.mSrc);
            if (ser == nullptr) {
                continue;
            }
            findFault(ser, QStringLiteral("系统"), info.mName);
        }
    }
}


int GFaultWidget::findFault(const GNodeInfo& info)
{
    const GHvacDataInfo::SeriesPtr ser = tp->row(info.mSrc);
    auto fr = std::find_if(ser->begin(), ser->end(), [](GHvacDataInfo::NumType i)->bool {
        return (qFuzzyCompare(i, 1));
    });

    if (fr != ser->end()) {
        int dcol = std::distance(ser->begin(), fr);
        int mr = m_model->rowCount();
        //位置
        m_model->setItem(mr, 0, new QStandardItem(occ));
        //故障名
        m_model->setItem(mr, 1, new QStandardItem(faultName));
        //故障时间

//        m_model->setItem(mr, 2, );
    }
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
