#include "GProjectArchivesWidget.h"
#include "ui_GProjectArchivesWidget.h"
#include <QDebug>
#include <QVariant>
#include <QHeaderView>
#include <QFileDialog>
#include "SACsvStream.h"
#include <QMessageBox>
#include "xlsxdocument.h"
GProjectArchivesWidget::GProjectArchivesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GProjectArchivesWidget)
{
    ui->setupUi(this);
    m_api = new GBigDataAPI(this);
    connect(m_api, &GBigDataAPI::projectInfos, this, &GProjectArchivesWidget::onRecProjectInfo);
    connect(m_api, &GBigDataAPI::projectFaults, this, &GProjectArchivesWidget::onRecProjectFaults);
    connect(m_api, &GBigDataAPI::projectMacChangeHistory, this, &GProjectArchivesWidget::onRecProjectMacChangeHistory);
    m_treeModel = new QStandardItemModel(this);
    m_tableModel = new GArchivesTableModel(this);
    ui->treeView->setModel(m_treeModel);
    ui->tableView->setModel(m_tableModel);
    ui->progressBar->setVisible(false);
    ui->progressBar->setRange(0, 3);
    ui->progressBar->setValue(0);
}


GProjectArchivesWidget::~GProjectArchivesWidget()
{
    delete ui;
}


void GProjectArchivesWidget::setProjectID(int proid)
{
    ui->lineEditInput->setText(QString::number(proid));
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);
    m_api->requestProjectInfo(proid);
    m_api->requestProjectFaults(proid);
    m_api->requesMacChangeHistory(proid);
    m_searchArg = QString::number(proid);
    resetModel();
}


void GProjectArchivesWidget::changeEvent(QEvent *e)
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


void GProjectArchivesWidget::onRecProjectInfo(QJsonObject res)
{
    stepProgress();
    if (!res.contains("errcode")) {
        return;
    }
    int errcode = res["errcode"].toInt();

    if (0 != errcode) {
        QString errstr;
        switch (errcode)
        {
        case 101:
            errstr = QStringLiteral("无工程信息");
            break;

        default:
            break;
        }
        emit message(QStringLiteral("%1的查询结果异常：%2").arg(m_searchArg).arg(errstr));
        return;
    }
    QVariantMap m = res.toVariantMap();
    QList<QStandardItem *> row = createRow(QStringLiteral("工程id"), m[QStringLiteral("工程id")].toString());

    m_treeModel->appendRow(row);
    row = createRow(QStringLiteral("系统id"), m[QStringLiteral("系统id")].toString());
    m_treeModel->appendRow(row);
    row = createRow(QStringLiteral("机组型号"), m[QStringLiteral("机组型号")].toString());
    m_treeModel->appendRow(row);
    row = createRow(QStringLiteral("省"), m[QStringLiteral("省")].toString());
    m_treeModel->appendRow(row);
    row = createRow(QStringLiteral("市"), m[QStringLiteral("市")].toString());
    m_treeModel->appendRow(row);
    row = createRow(QStringLiteral("工程定位名称"), m[QStringLiteral("工程定位名称")].toString());
    m_treeModel->appendRow(row);
    row = createRow(QStringLiteral("工程详细地址"), m[QStringLiteral("工程详细地址")].toString());
    m_treeModel->appendRow(row);
    row = createRow(QStringLiteral("工程创建时间"), m[QStringLiteral("工程创建时间")].toString());
    m_treeModel->appendRow(row);

    //用户信息
    QStandardItem *itemUser = new QStandardItem(QStringLiteral("用户信息"));

    //用户信息 - 业主姓名
    row = createRow(QStringLiteral("业主姓名"), m[QStringLiteral("业主姓名")].toString());
    itemUser->appendRow(row);
    //用户信息 - 业主联系方式
    row = createRow(QStringLiteral("业主联系方式"), m[QStringLiteral("业主联系方式")].toString());
    itemUser->appendRow(row);
    //用户信息 - 业主联系方式
    row = createRow(QStringLiteral("销售公司"), m[QStringLiteral("销售公司")].toString());
    itemUser->appendRow(row);
    //用户信息 - 安装工
    row = createRow(QStringLiteral("安装工"), m[QStringLiteral("安装工")].toString());
    itemUser->appendRow(row);
    //用户信息 - 安装工联系方式
    row = createRow(QStringLiteral("安装工联系方式"), m[QStringLiteral("安装工联系方式")].toString());
    itemUser->appendRow(row);
    //用户信息 - 安装工反馈地址
    row = createRow(QStringLiteral("安装工反馈地址"), m[QStringLiteral("安装工反馈地址")].toString());
    itemUser->appendRow(row);
    m_treeModel->appendRow(itemUser);
    //模块信息
    QStandardItem *itemModule = new QStandardItem(QStringLiteral("模块信息"));
    //模块信息 - 在线模块mac
    QStandardItem *item = new QStandardItem(QStringLiteral("在线模块mac"));
    QList<QVariant> onlinemac = m[QStringLiteral("在线模块mac")].toList();

    for (int i = 0; i < onlinemac.size(); ++i)
    {
        row = createRow(QStringLiteral("mac%1").arg(i+1), onlinemac[i].toString());
        item->appendRow(row);
    }
    itemModule->appendRow(item);
    //模块信息 - 不在线模块mac
    QList<QVariant> offlinemac = m[QStringLiteral("不在线模块mac")].toList();

    if (!offlinemac.isEmpty()) {
        item = new QStandardItem(QStringLiteral("不在线模块mac"));
        for (int i = 0; i < offlinemac.size(); ++i)
        {
            row = createRow(QStringLiteral("mac%1").arg(i+1), offlinemac[i].toString());
            item->appendRow(row);
        }
        itemModule->appendRow(item);
    }
    //模块信息 - imei
    row = createRow(QStringLiteral("imei"), m[QStringLiteral("imei")].toString());
    itemModule->appendRow(row);
    //模块信息 - 手机序列号
    row = createRow(QStringLiteral("手机序列号"), m[QStringLiteral("手机序列号")].toString());
    itemModule->appendRow(row);
    //模块信息 - 模块软件版本
    row = createRow(QStringLiteral("模块软件版本"), m[QStringLiteral("模块软件版本")].toString());
    itemModule->appendRow(row);
    //模块信息 - 模块软件版本
    row = createRow(QStringLiteral("模块硬件版本"), m[QStringLiteral("模块硬件版本")].toString());
    itemModule->appendRow(row);
    //模块信息 - dtu更新时间
    row = createRow(QStringLiteral("dtu更新时间"), m[QStringLiteral("dtu更新时间")].toString());
    itemModule->appendRow(row);
    m_treeModel->appendRow(itemModule);
    //设备信息
    QStandardItem *itemDevice = new QStandardItem(QStringLiteral("设备信息"));
    QStandardItem *oduDevice = new QStandardItem(QStringLiteral("外机(在线)"));
    QStandardItem *iduDevice = new QStandardItem(QStringLiteral("内机(在线)"));
    QStandardItem *offlineDevice = new QStandardItem(QStringLiteral("不在线设备信息"));
    QList<QVariant> deviceList = m[QStringLiteral("设备信息")].toList();

    //迭代设备
    for (const QVariant& dev : deviceList)
    {
        QVariantMap devobj = dev.toMap();
        if (!devobj.contains("canip")) {
            continue;
        }
        int canip = devobj["canip"].toInt();
        if (devobj[QStringLiteral("设备是否在线")].toInt() == 1) {
            if (canip >= 12) {
                //内机
                QStandardItem *iduItem = new QStandardItem(QStringLiteral("canip%1").arg(devobj[QStringLiteral("canip")].toInt()));
                iduDevice->appendRow(iduItem);
                row = createRow(QStringLiteral("条码"), devobj[QStringLiteral("条码")].toString());
                iduItem->appendRow(row);
                row = createRow(QStringLiteral("额定容量"), devobj[QStringLiteral("额定容量")].toString());
                iduItem->appendRow(row);
                row = createRow(QStringLiteral("主控MAC"), devobj[QStringLiteral("主控MAC")].toString());
                iduItem->appendRow(row);
            }else{
                //外机
                QStandardItem *oduItem = new QStandardItem(QStringLiteral("canip%1").arg(devobj[QStringLiteral("canip")].toInt()));
                oduDevice->appendRow(oduItem);
                row = createRow(QStringLiteral("条码"), devobj[QStringLiteral("条码")].toString());
                oduItem->appendRow(row);
                row = createRow(QStringLiteral("额定容量"), devobj[QStringLiteral("额定容量")].toString());
                oduItem->appendRow(row);
                row = createRow(QStringLiteral("主控MAC"), devobj[QStringLiteral("主控MAC")].toString());
                oduItem->appendRow(row);
                row = createRow(QStringLiteral("控制器版本"), devobj[QStringLiteral("控制器版本")].toString());
                oduItem->appendRow(row);
            }
        }else{
            //不在线设备
            if (canip >= 12) {
                //内机
                QStandardItem *iduItem = new QStandardItem(QStringLiteral("内机-canip %1").arg(devobj[QStringLiteral("canip")].toInt()));
                offlineDevice->appendRow(iduItem);
                row = createRow(QStringLiteral("条码"), devobj[QStringLiteral("条码")].toString());
                iduItem->appendRow(row);
                row = createRow(QStringLiteral("额定容量"), devobj[QStringLiteral("额定容量")].toString());
                iduItem->appendRow(row);
                row = createRow(QStringLiteral("主控MAC"), devobj[QStringLiteral("主控MAC")].toString());
                iduItem->appendRow(row);
            }else{
                //外机
                QStandardItem *oduItem = new QStandardItem(QStringLiteral("外机-canip%1").arg(devobj[QStringLiteral("canip")].toInt()));
                offlineDevice->appendRow(oduItem);
                row = createRow(QStringLiteral("条码"), devobj[QStringLiteral("条码")].toString());
                oduItem->appendRow(row);
                row = createRow(QStringLiteral("额定容量"), devobj[QStringLiteral("额定容量")].toString());
                oduItem->appendRow(row);
                row = createRow(QStringLiteral("主控MAC"), devobj[QStringLiteral("主控MAC")].toString());
                oduItem->appendRow(row);
                row = createRow(QStringLiteral("控制器版本"), devobj[QStringLiteral("控制器版本")].toString());
                oduItem->appendRow(row);
            }
        }
    }
    itemDevice->appendRows({ oduDevice, iduDevice, offlineDevice });
    m_treeModel->appendRow(itemDevice);
    //展开所有
    ui->treeView->expandAll();
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);


    //查询维修单信息
    QVariantList repairs = m[QStringLiteral("维修单信息")].toList();

    if (repairs.size() > 0) {
        for (const QVariant& v : repairs)
        {
            QVariantMap obj = v.toMap();
            if (!obj.contains(QStringLiteral("报修日期"))) {
                continue;
            }
            QDateTime dt = QDateTime::fromString(
                obj[QStringLiteral("报修日期")].toString()
                , "yyyy-MM-ddTHH:mm:ss");
            GArchivesTableModel::ArchiveInfo info;
            info.mType = QStringLiteral("维修记录");
            info.mBackColor = QColor(245, 49, 39, 140);
            info.mInfo = QStringLiteral("条码:%1"
                "\n故障原因:%2"
                "\n维修工名称:%3"
                "\n维修工手机号:%4"
                "\n用户名称:%5"
                "\n用户电话:%6"
                )
                .arg(obj[QStringLiteral("条码")].toString())
                .arg(obj[QStringLiteral("故障原因")].toString())
                .arg(obj[QStringLiteral("维修工名称")].toString())
                .arg(obj[QStringLiteral("维修工手机号")].toString())
                .arg(obj[QStringLiteral("用户名称")].toString())
                .arg(obj[QStringLiteral("用户电话")].toString())
            ;
            info.mIcon = QIcon(":/icon/icon/fixed.svg");
            m_tableModel->insertInfo(dt, info);
        }
    }
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void GProjectArchivesWidget::onRecProjectFaults(QJsonArray res)
{
    stepProgress();
    QVariantList m = res.toVariantList();

    for (const QVariant& v : m)
    {
        QVariantMap obj = v.toMap();
        if (!obj.contains(QStringLiteral("RecordTime"))) {
            continue;
        }
        QDateTime dt = QDateTime::fromString(
            obj[QStringLiteral("RecordTime")].toString()
            , "yyyy-MM-ddTHH:mm:ss");
        int debugStatus = obj[QStringLiteral("DebugStatus")].toInt();
        QString strDebugStatus = ((debugStatus == 0 || debugStatus == 16) ? QStringLiteral("调试") : QStringLiteral("非调试"));
        QString faultLevel = obj[QStringLiteral("FaultLevel")].toString();
        GArchivesTableModel::ArchiveInfo info;
        if (faultLevel == QStringLiteral("三级")) {
            info.mBackColor = QColor(255, 167, 106, 140);
        }
        info.mType = QStringLiteral("故障上报");
        info.mInfo = QStringLiteral("故障名称:%1"
            "\n故障等级:%2"
            "\n状态:%3"
            "\n故障位置:%4"
            )
            .arg(obj[QStringLiteral("FaultDetails")].toString())
            .arg(faultLevel)
            .arg(strDebugStatus)
            .arg(obj[QStringLiteral("FaultType")].toString())
        ;
        info.mIcon = QIcon(":/icon/icon/fault.svg");
        m_tableModel->insertInfo(dt, info);
    }
    m_tableModel->update();
    m_tableModel->sort(0);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void GProjectArchivesWidget::onRecProjectMacChangeHistory(QJsonObject res)
{
    stepProgress();
    if (!res.contains("statue")) {
        return;
    }
    bool issucceed = res["statue"].toBool();

    if (!issucceed) {
        return;
    }
    if (!res.contains("datas")) {
        return;
    }
    QVariantList m = res["datas"].toArray().toVariantList();

    for (const QVariant& v : m)
    {
        QVariantMap obj = v.toMap();
        if (!obj.contains(QStringLiteral("recordtime"))) {
            continue;
        }
        QDateTime dt = QDateTime::fromString(
            obj[QStringLiteral("recordtime")].toString()
            , "yyyy-MM-ddTHH:mm:ss");
        QString oldmac = obj[QStringLiteral("old_mac_addr")].toString();
        QString newmac = obj[QStringLiteral("new_mac_addr")].toString();
        QString new_barcode = obj[QStringLiteral("new_barcode")].toString();
        GArchivesTableModel::ArchiveInfo info;
        info.mType = QStringLiteral("换版");
        info.mInfo = QStringLiteral("旧MAC:%1\n"
            "新MAC:%2\n"
            "新条码:%3").arg(oldmac).arg(newmac).arg(new_barcode);
        info.mIcon = QIcon(":/icon/icon/changeMac.svg");
        info.mBackColor = QColor(224, 240, 255);
        m_tableModel->insertInfo(dt, info);
    }
    m_tableModel->update();
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void GProjectArchivesWidget::resetModel()
{
    m_treeModel->clear();
    m_treeModel->setHorizontalHeaderLabels({ QStringLiteral("参数"), QStringLiteral("值") });
    m_tableModel->clear();
}


QList<QStandardItem *> GProjectArchivesWidget::createRow(const QString& key, const QString& value)
{
    QStandardItem *k = new QStandardItem(key);
    QStandardItem *v = new QStandardItem(value);

    return { k, v };
}


void GProjectArchivesWidget::stepProgress()
{
    int provalue = ui->progressBar->value();

    ui->progressBar->setValue(++provalue);
    if (provalue >= ui->progressBar->maximum()) {
        ui->progressBar->setVisible(false);
    }
}


void GProjectArchivesWidget::on_pushButtonSearch_clicked()
{
    bool isok = false;
    int pid = ui->lineEditInput->text().toInt(&isok);

    if (!isok) {
        emit message(QStringLiteral("工程id输入异常"));
        return;
    }
    setProjectID(pid);
}


void GProjectArchivesWidget::on_pushButtonExport_clicked()
{
    if (0 == m_tableModel->count()) {
        QMessageBox::warning(this, QStringLiteral("警告")
            , QStringLiteral("无内容可导出"));
        return;
    }
    QString fn = QFileDialog::getSaveFileName(this, QStringLiteral("选择导出的文件")
        , QString()
        , QStringLiteral("xlsx文件(*.xlsx)")
        );

    if (fn.isEmpty()) {
        return;
    }
    QFile file(fn);

    if (!file.open(QIODevice::ReadWrite)) {
    }
    QXlsx::Document xlsx;
    QXlsx::Format font;

    font.setFontBold(true);
    font.setFontSize(16);
    font.setBorderStyle(QXlsx::Format::BorderThick);
    xlsx.addSheet(QStringLiteral("全生命周期"));
    xlsx.write(1, 1, QStringLiteral("日期"), font);
    xlsx.write(1, 2, QStringLiteral("类型"), font);
    xlsx.write(1, 3, QStringLiteral("事件"), font);
    for (int i = 0; i < m_tableModel->count(); ++i)
    {
        auto d = m_tableModel->getAt(i);
        QXlsx::Format fmt;
        fmt.setFontSize(12);
        font.setBorderStyle(QXlsx::Format::BorderThick);
        if (d.second.mBackColor.isValid()) {
            fmt.setPatternBackgroundColor(d.second.mBackColor);
        }
        xlsx.write(i+2, 1, d.first, fmt);
        xlsx.write(i+2, 2, d.second.mType, fmt);
        xlsx.write(i+2, 3, d.second.mInfo, fmt);
    }
    if (!xlsx.saveAs(fn)) {
        QMessageBox::critical(this, QStringLiteral("警告")
            , QStringLiteral("保存文件%1失败").arg(fn));
        return;
    }
    QMessageBox::information(this, QStringLiteral("信息")
        , QStringLiteral("保存文件%1成功").arg(fn));
    return;
}
