#include "GListAllMonitorDialog.h"
#include "ui_GListAllMonitorDialog.h"
#include <QDebug>
#include <QItemSelectionModel>
GListAllMonitorDialog::GListAllMonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GListAllMonitorDialog)
    , m_api(nullptr)
{
    ui->setupUi(this);
    m_tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(m_tableModel);
}


GListAllMonitorDialog::~GListAllMonitorDialog()
{
    delete ui;
}


void GListAllMonitorDialog::setupAPI(GBigDataAPI *api)
{
    if ((api == m_api) || (nullptr == api)) {
        return;
    }

    if (!m_api.isNull()) {
        disconnect(m_api.data(), &GBigDataAPI::listAllMonitorInfos, this, &GListAllMonitorDialog::onRecAllMonitorInfos);
    }
    m_api = api;
    connect(api, &GBigDataAPI::listAllMonitorInfos, this, &GListAllMonitorDialog::onRecAllMonitorInfos);
}


void GListAllMonitorDialog::updateInfo()
{
    if (m_api.isNull()) {
        return;
    }
    m_api->requesListAllMonitorInfos();
}


QString GListAllMonitorDialog::getSelectMac() const
{
    int r = getSelectRow();

    if (r < 0) {
        return (QString());
    }
    return (m_tableModel->item(r)->text());
}


int GListAllMonitorDialog::getSelectRow() const
{
    QItemSelectionModel *selModel = ui->tableView->selectionModel();

    if (!selModel) {
        return (-1);
    }
    QModelIndexList sr = selModel->selectedRows();

    if (sr.size() != 1) {
        return (-1);
    }
    return (sr[0].row());
}


int GListAllMonitorDialog::getSelectProjectID() const
{
    int r = getSelectRow();

    if (r < 0) {
        return (-1);
    }
    bool ok = false;
    int id = (m_tableModel->item(r, 1)->text()).toInt(&ok);

    return (ok ? id : -1);
}


void GListAllMonitorDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

    default:
        break;
    }
}


void GListAllMonitorDialog::clearTable()
{
    m_tableModel->clear();
    m_tableModel->setHorizontalHeaderLabels(
    {
        QStringLiteral("模块mac"),
        QStringLiteral("工程id"),
        QStringLiteral("机型"),
        QStringLiteral("机型详情"),
        QStringLiteral("IMEI"),
        QStringLiteral("IMSI"),
        QStringLiteral("手机序列号"),
        QStringLiteral("手机号码"),
        QStringLiteral("软件版本号"),
        QStringLiteral("硬件版本号"),
        QStringLiteral("定位")
    });
}


void GListAllMonitorDialog::on_toolButtonUpdate_clicked()
{
    updateInfo();
}


void GListAllMonitorDialog::onRecAllMonitorInfos(QJsonObject res)
{
    QVariantMap org = res.toVariantMap();

    if (!org.contains("statue")) {
        return;
    }
    bool isOk = org["statue"].toBool();

    if (!isOk) {
        qWarning() << QStringLiteral("监控列表查询结果异常");
        return;
    }
    if (!org.contains("data")) {
        qWarning() << QStringLiteral("监控列表返回结果异常");
        return;
    }
    clearTable();
    QVariantMap m = org["data"].toMap();

    for (auto i = m.begin(); i != m.end(); ++i)
    {
        QVariantMap vmap = i.value().toMap();
        QList<QStandardItem *> rows;
        rows.append(new QStandardItem(i.key()));                                //mac
        rows.append(new QStandardItem(QString::number(vmap["proid"].toInt()))); //工程id
        rows.append(new QStandardItem(vmap["type"].toString()));                //机型
        rows.append(new QStandardItem(vmap["machinetype"].toString()));         //机型详情
        rows.append(new QStandardItem(vmap["imei"].toString()));                //IMEI
        rows.append(new QStandardItem(vmap["imsi"].toString()));                //IMSI
        rows.append(new QStandardItem(vmap["phone_series_num"].toString()));    //手机序列号
        rows.append(new QStandardItem(vmap["phone_num"].toString()));           //手机号码
        rows.append(new QStandardItem(vmap["soft_version"].toString()));        //软件版本号
        rows.append(new QStandardItem(vmap["hard_version"].toString()));        //硬件版本号
        rows.append(new QStandardItem(vmap["local"].toString()));               //定位
        m_tableModel->appendRow(rows);
    }
}
