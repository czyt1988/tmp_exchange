#include "GPlotWidget.h"
#include "ui_GPlotWidget.h"
#include "SATable.h"
#include <QDebug>
#define ROLE_ITEM_DATA_HVACTYPE		(Qt::UserRole+1)
#define ITEM_DATA_TYPE_System		1
#define ITEM_DATA_TYPE_Module		2
#define ITEM_DATA_TYPE_Idu		3
#define ROLE_ITEM_DATA_SRC		(Qt::UserRole+2)


GPlotWidget::GPlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlotWidget),
    mTemplate(nullptr)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 4);
    m_treeModel = new QStandardItemModel(this);
    ui->treeView->setModel(m_treeModel);
}


GPlotWidget::~GPlotWidget()
{
    delete ui;
}


void GPlotWidget::setData(const GHvacDataInfo& d)
{
    m_data = d;
    resetTreeView();
}


void GPlotWidget::setTemplate(GTemplate *temp)
{
    mTemplate = temp;
    resetTreeView();
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
                baseItem->appendRow(item);
            }
            //
            m_treeModel->appendRow(baseItem);
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
