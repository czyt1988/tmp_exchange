#include "GDataReviewWidget.h"
#include "ui_GDataReviewWidget.h"
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QScopedPointer>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>

GDataReviewWidget::GDataReviewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GDataReviewWidget)
    , mCurrentSpeed(Speed1x)
    , mMode(NoneMode)
    , mCurrentTemplate(nullptr)
{
    ui->setupUi(this);
    connect(&mTimer, &QTimer::timeout, this, &GDataReviewWidget::onTimeout);

    mModuleWidget = new GModuleValueView(this);
    ui->scrollArea->setWidget(mModuleWidget);
    ui->splitterSysModule->setStretchFactor(0, 1);
    ui->splitterSysModule->setStretchFactor(1, 5);
    ui->splitterModuleAndIdu->setStretchFactor(0, 3);
    ui->splitterModuleAndIdu->setStretchFactor(1, 1);
    setStopMode();
    setSpeed(Speed1x);
    showMaximized();
}


GDataReviewWidget::~GDataReviewWidget()
{
    delete ui;
}


void GDataReviewWidget::setTemplate(GTemplate *t)
{
    if (!t) {
        return;
    }
    mCurrentTemplate = t;
    ui->treeViewSystem->setModel(t->getSystemModel());
    ui->treeViewSystem->expandAll();
    ui->tableViewIdu->setModel(t->getIduModel());
    mModuleWidget->setNodeInfos(t->getModuleInfoList());
    setWindowTitle(QStringLiteral("数据回放 - 模板：%1").arg(t->name()));
    if (mCurrentTemplate) {
        if (!mHvacInfo.iduCanIPs.isEmpty()) {
            mCurrentTemplate->getIduModel()->setCanIps(mHvacInfo.iduCanIPs);
        }
    }
    qDebug() << QStringLiteral("设置模板成功");
}


GTemplate *GDataReviewWidget::getCurrentTemplate() const
{
    return (mCurrentTemplate);
}


/**
 * @brief 获取当前模式
 * @return
 */
GDataReviewWidget::Mode GDataReviewWidget::getMode() const
{
    return (mMode);
}


void GDataReviewWidget::startOpenReviewData()
{
    if (mTimer.isActive()) {
        mTimer.stop();
    }
    setNoneMode();
}


void GDataReviewWidget::setData(GHvacDataInfo datainfo)
{
    mHvacInfo = datainfo;

    //设置进度条
    //设置module的canip
    ui->comboBoxCanIP->clear();
    for (int i : mHvacInfo.moduleCanIPs)
    {
        ui->comboBoxCanIP->addItem(QString::number(i), i);
    }
    //设置范围
    if (mHvacInfo.allDateTimeScale.size() <= 0) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("无有效数据"));
        return;
    }
    //设置范围
    ui->labelStartTime->setText(mHvacInfo.allDateTimeScale.first().toString("yyyy-MM-dd HH:mm:ss"));
    ui->labelEndTime->setText(mHvacInfo.allDateTimeScale.back().toString("yyyy-MM-dd HH:mm:ss"));
    ui->horizontalSlider->setRange(0, mHvacInfo.allDateTimeScale.size());
    ui->horizontalSlider->setSingleStep(1);
    //更新模板的canip
    if (mCurrentTemplate) {
        mCurrentTemplate->getIduModel()->setCanIps(datainfo.iduCanIPs);
    }
}


void GDataReviewWidget::toIndex(int i)
{
    ui->horizontalSlider->setValue(i);
}


void GDataReviewWidget::changeEvent(QEvent *e)
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


void GDataReviewWidget::onTimeout()
{
    int v = ui->horizontalSlider->value();
    int s = ui->horizontalSlider->singleStep();

    ui->horizontalSlider->setValue(v+s);
}


void GDataReviewWidget::updateValue(int msecsSinceEpoch)
{
    //首先更新系统数据
    QJsonObject json = mHvacInfo.get(msecsSinceEpoch);

    valueRender(json);
}


void GDataReviewWidget::setRunMode()
{
    mMode = RuningMode;
    mTimer.start();
}


void GDataReviewWidget::setStopMode()
{
    mMode = StoppedMode;
    mTimer.stop();
}


void GDataReviewWidget::setNoneMode()
{
    mMode = NoneMode;
}


void GDataReviewWidget::setSpeed(GDataReviewWidget::Speed s)
{
    mCurrentSpeed = s;
    switch (s)
    {
    case Speed1x:
    {
        mTimer.setInterval(1000);
    }
    break;

    case Speed2x:
    {
        mTimer.setInterval(500);
    }
    break;

    case Speed3x:
    {
        mTimer.setInterval(300);
    }
    break;

    case SpeedMax:
    {
        mTimer.setInterval(0);
    }

    default:
        break;
    }
}


void GDataReviewWidget::updateValueBySliderValue(int value)
{
    if ((value >= mHvacInfo.allDateTimeScale.size()) || (value < 0)) {
        qDebug() << "value out range";
        if (mMode == RuningMode) {
            setStopMode();
        }
        return;
    }
    QDateTime dt = mHvacInfo.allDateTimeScale.at(value);

    int msecsSinceEpoch = dt.toMSecsSinceEpoch();

    ui->labelCurrentTime->setText(QStringLiteral("当前时间:%1").arg(dt.toString("yyyy-MM-dd HH:mm:ss")));
    updateValue(msecsSinceEpoch);
}


void GDataReviewWidget::on_horizontalSlider_valueChanged(int value)
{
    updateValueBySliderValue(value);
}


void GDataReviewWidget::valueRender(const QJsonObject& obj)
{
//    QJsonDocument doc(obj);
//    QString str = QString(doc.toJson(QJsonDocument::Indented));

//    QFile f("./VRFBigDataView.json");

//    if (f.open(QIODevice::ReadWrite|QIODevice::Append)) {
//        QTextStream s(&f);
//        s << str;
//    }


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
        i = sys.find(n.mSrc.toLower());
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
        i = modulewithip.find(n.mSrc.toLower());
        if (i == modulewithip.constEnd()) {
            qDebug() << QStringLiteral("无法在module中找到参数key:") << n.mSrc.toLower();
            continue;
        }
        n.mDisplayValue = (*i).toString();
        mModuleWidget->updateValue(n);
    }

    //渲染内机

    QJsonObject::const_iterator i = obj.find("idu");

    if (i == obj.constEnd()) {
        qDebug() << QStringLiteral("无法找到idu");
        return;
    }
    QList<GNodeInfo> idunodes = mCurrentTemplate->getIduModel()->getNodeInfo();
    QJsonObject iduobj = (*i).toObject();

    for (int canip : mHvacInfo.iduCanIPs)
    {
        QString kname = QString("idu_%1").arg(canip);
        i = iduobj.find(kname);
        if (i == iduobj.constEnd()) {
            qDebug() << QStringLiteral("无法找到canip:")<<canip<<QStringLiteral("的内容");
            continue;
        }
        QJsonObject iduwithip = (*i).toObject();
        for (GNodeInfo& n : idunodes)
        {
            i = iduwithip.find(n.mSrc.toLower());
            //所有canip遍历一遍
            if (i == iduwithip.constEnd()) {
                qDebug() << QStringLiteral("无法在%1中找到参数key:").arg(kname) << n.mSrc.toLower();
                continue;
            }
            n.mDisplayValue = (*i).toString();
        }
        mCurrentTemplate->getIduModel()->updateValue(idunodes);
    }
}


/**
 * @brief MainWidget::on_comboBoxCanIP_currentIndexChanged
 * @param arg1
 */
void GDataReviewWidget::on_comboBoxCanIP_currentIndexChanged(const QString& arg1)
{
    Q_UNUSED(arg1);
    int value = ui->horizontalSlider->value();

    updateValueBySliderValue(value);
}
