#include "GMonitorWidget.h"
#include "ui_GMonitorWidget.h"
#include "GBigDataAPI.h"
#include <QElapsedTimer>
GMonitorWidget::GMonitorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GMonitorWidget)
    , mCurrentTemplate(nullptr)
    , mApi(nullptr)
    , m_interval(6000)
{
    ui->setupUi(this);
    mModuleWidget = new GModuleValueView(this);
    ui->scrollArea->setWidget(mModuleWidget);
    ui->splitterSysModule->setStretchFactor(0, 1);
    ui->splitterSysModule->setStretchFactor(1, 5);
    ui->splitterModuleAndIdu->setStretchFactor(0, 3);
    ui->splitterModuleAndIdu->setStretchFactor(1, 1);
    connect(&mTimer, &QTimer::timeout, this, &GMonitorWidget::onTimeout);
    setInterval(m_interval);
    ui->comboBoxCanIP->addItem("8");
    ui->comboBoxCanIP->addItem("9");
    ui->comboBoxCanIP->addItem("10");
    ui->comboBoxCanIP->addItem("11");
}


GMonitorWidget::~GMonitorWidget()
{
    delete ui;
}


void GMonitorWidget::startMonitor(const QString& mac)
{
    setMac(mac);
    m_startDatetime = QDateTime::currentDateTime();
    onTimeout();
    mTimer.start();
}


bool GMonitorWidget::startMonitor()
{
    if (m_mac.isEmpty()) {
        return (false);
    }
    startMonitor(m_mac);
    return (true);
}


void GMonitorWidget::stopMonitor()
{
    mTimer.stop();
}


bool GMonitorWidget::isStart() const
{
    return (mTimer.isActive());
}


void GMonitorWidget::setMac(const QString& mac)
{
    m_mac = mac;
}


int GMonitorWidget::interval() const
{
    return (mTimer.interval());
}


void GMonitorWidget::setInterval(int ms)
{
    mTimer.setInterval(ms);
}


void GMonitorWidget::setTemplate(GTemplate *t)
{
    if (!t) {
        return;
    }
    mCurrentTemplate = t;
    ui->treeViewSystem->setModel(t->getSystemModel());
    ui->treeViewSystem->expandAll();
    ui->tableViewIdu->setModel(t->getIduModel());
    mModuleWidget->clear();
    mModuleWidget->setNodeInfos(t->getModuleInfoList());
    setWindowTitle(QStringLiteral("监控 - 模板：%1").arg(t->name()));
    qDebug() << QStringLiteral("设置监控模板成功");
}


void GMonitorWidget::setupAPI(GAPIManager *api)
{
    mApi = api;
    connect(api, &GAPIManager::monitorData, this, &GMonitorWidget::acceptMonitorData);
}


QString GMonitorWidget::mac() const
{
    return (m_mac);
}


/**
 * @brief 获取监控持续时间
 * @return
 */
int GMonitorWidget::lastMinute() const
{
    return (m_startDatetime.secsTo(QDateTime::currentDateTime()) / 60.0);
}


void GMonitorWidget::changeEvent(QEvent *e)
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


void GMonitorWidget::acceptMonitorData(const QJsonObject& obj)
{
    if (!obj.contains("statue")) {
        return;
    }
    bool isOk = obj.value("statue").toBool();

    if (!isOk) {
        emit message(QStringLiteral("%1的监控结果异常").arg(m_mac));
        return;
    }
    if (!obj.contains("data")) {
        emit message(QStringLiteral("%1的监控结果异常2").arg(m_mac));
        return;
    }
    QJsonObject data = obj["data"].toObject();

    if (data.isEmpty()) {
        return;
    }
    valueRender(data);
}


void GMonitorWidget::onTimeout()
{
    if (mApi == nullptr) {
        return;
    }
    mApi->requestMonitorData(m_mac);
}


void GMonitorWidget::valueRender(const QJsonObject& obj)
{
    if (mCurrentTemplate == nullptr) {
        emit message(QStringLiteral("模板异常"));
        return;
    }
    //渲染到system
    const QList<GNodeInfo>& sysnodes = mCurrentTemplate->getSystemInfoList();

    for (GNodeInfo n : sysnodes)
    {
        QJsonObject::const_iterator i = obj.find("system");
        if (i == obj.constEnd()) {
            continue;
        }
        QJsonObject sys = (*i).toObject();
        i = sys.find(n.mSrc);
        if (i == sys.constEnd()) {
            qDebug() << QStringLiteral("无法在system中找到参数key:") << n.mSrc.toLower();
            continue;
        }
        n.mDisplayValue = (*i).toString();
        mCurrentTemplate->updateValue(n);
    }
    //渲染到module
    const QList<GNodeInfo>& modulenodes = mCurrentTemplate->getModuleInfoList();

    for (GNodeInfo n : modulenodes)
    {
        QJsonObject::const_iterator i = obj.find("module");
        if (i == obj.constEnd()) {
            continue;
        }
        QJsonObject module = (*i).toObject();
        i = module.find(QString("module_")+ui->comboBoxCanIP->currentText());
        if (i == module.constEnd()) {
            qDebug() << QStringLiteral("无法找到canip:")<<ui->comboBoxCanIP->currentText()<<QStringLiteral("的内容");
            continue;
        }
        QJsonObject modulewithip = (*i).toObject();
        i = modulewithip.find(n.mSrc);
        if (i == modulewithip.constEnd()) {
            qDebug() << QStringLiteral("无法在module中找到参数key:") << n.mSrc.toLower();
            continue;
        }
        n.mDisplayValue = (*i).toString();
        mModuleWidget->updateValue(n);
    }

    //渲染内机

    QJsonObject::const_iterator iduite = obj.find("idu");

    if (iduite == obj.constEnd()) {
        qDebug() << QStringLiteral("无法找到idu");
        return;
    }
    QList<GNodeInfo> idunodes = mCurrentTemplate->getIduModel()->getNodeInfo();
    QJsonObject iduobj = (*iduite).toObject();
    QVariantMap idumaps = iduobj.toVariantMap();

    for (auto im = idumaps.begin(); im != idumaps.end(); ++im)
    {
        QString name = im.key();
        QStringList vs = name.split("_");
        if (vs.size() != 2) {
            qDebug() << QStringLiteral("内机key异常");
            continue;
        }
        bool isok = false;
        int canip = vs[1].toInt(&isok);
        if (!isok) {
            qDebug() << QStringLiteral("内机canip异常") << vs[1];
            continue;
        }
        QVariantMap valueMap = im.value().toMap();
        for (GNodeInfo& n : idunodes)
        {
            auto vm = valueMap.find(n.mSrc);
            //所有canip遍历一遍
            if (vm == valueMap.constEnd()) {
                qDebug() << QStringLiteral("无法在%1中找到参数key:").arg(name) << n.mSrc.toLower();
                continue;
            }
            n.mDisplayValue = (*vm).toString();
        }
        mCurrentTemplate->getIduModel()->updateValue(canip, idunodes);
    }
}
