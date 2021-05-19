#include "MainWidget.h"
#include "ui_MainWidget.h"
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QScopedPointer>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
    , mCurrentSpeed(Speed1x)
    , mMode(NoneMode)
    , mSpeed1(":/icon/icon/1x.svg")
    , mSpeed2(":/icon/icon/2x.svg")
    , mSpeed3(":/icon/icon/3x.svg")
    , mCurrentTemplate(nullptr)
{
    ui->setupUi(this);
    connect(&mTimer, &QTimer::timeout, this, &MainWidget::onTimeout);

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


MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::setTemplate(GTemplate *t)
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



GTemplate *MainWidget::getCurrentTemplate() const
{
    return (mCurrentTemplate);
}

/**
 * @brief 获取当前模式
 * @return
 */
MainWidget::Mode MainWidget::getMode() const
{
    return mMode;
}


void MainWidget::open()
{
    bool ispause = false;

    if (mTimer.isActive()) {
        mTimer.stop();
        ispause = true;
    }
    QString filepath = QFileDialog::getOpenFileName(this, QStringLiteral("选择数据文件"), "", "数据文件 (*.zip)");

    if (filepath.isEmpty()) {
        if (ispause) {
            mTimer.start();
        }
        return;
    }
    GHvacDataFileIO *io = new GHvacDataFileIO();
    //设置相关信息

    QThread *readThread = new QThread();

    io->moveToThread(readThread);
    connect(readThread, &QThread::finished, io, &GHvacDataFileIO::deleteLater);
    connect(readThread, &QThread::finished, io, &QThread::deleteLater);
    //业务逻辑
    connect(this, &MainWidget::openFile, io, &GHvacDataFileIO::open);
    connect(io, &GHvacDataFileIO::readed, this, &MainWidget::onFileReaded);
    connect(io, &GHvacDataFileIO::message, this, &MainWidget::message);
    connect(io, &GHvacDataFileIO::error, this, &MainWidget::onIoError);
    //唤起线程自杀
    connect(io, &GHvacDataFileIO::readed, this, [readThread](GHvacDataInfo) {
        readThread->quit();
    });
    connect(io, &GHvacDataFileIO::error, this, [readThread](QString) {
        readThread->quit();
    });
    readThread->start();
    emit message(QStringLiteral("正打开:%1").arg(filepath));
    emit openFile(filepath);

    setNoneMode();
}


void MainWidget::toIndex(int i)
{
    ui->horizontalSlider->setValue(i);
}



void MainWidget::changeEvent(QEvent *e)
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


void MainWidget::onFileReaded(GHvacDataInfo info)
{
    mHvacInfo = info;

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
        mCurrentTemplate->getIduModel()->setCanIps(info.iduCanIPs);
    }
    //设置timmer

    emit fileReaded(info);
}


void MainWidget::onIOErrRec(const QString& msg)
{
    emit message(msg);
}


void MainWidget::onTimeout()
{
    int v = ui->horizontalSlider->value();
    int s = ui->horizontalSlider->singleStep();

    ui->horizontalSlider->setValue(v+s);
}


void MainWidget::updateValue(int msecsSinceEpoch)
{
    //首先更新系统数据
    QJsonObject json = mHvacInfo.get(msecsSinceEpoch);

    valueRender(json);
}






void MainWidget::setRunMode()
{
    mMode = RuningMode;
    mTimer.start();
}


void MainWidget::setStopMode()
{
    mMode = StoppedMode;
    mTimer.stop();
}


void MainWidget::setNoneMode()
{
    mMode = NoneMode;
}


void MainWidget::setSpeed(MainWidget::Speed s)
{
    mCurrentSpeed = s;
    switch (s)
    {
    case Speed1x:
    {
        ui->labelspeed->setPixmap(mSpeed1.pixmap(ui->labelspeed->size()));
        mTimer.setInterval(1000);
    }
    break;

    case Speed2x:
    {
        ui->labelspeed->setPixmap(mSpeed2.pixmap(ui->labelspeed->size()));
        mTimer.setInterval(500);
    }
    break;

    case Speed3x:
    {
        ui->labelspeed->setPixmap(mSpeed3.pixmap(ui->labelspeed->size()));
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


void MainWidget::updateValueBySliderValue(int value)
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


void MainWidget::on_pushButtonSpeed_clicked()
{
    if (Speed1x == mCurrentSpeed) {
        setSpeed(Speed2x);
    }else if (Speed2x == mCurrentSpeed) {
        setSpeed(Speed3x);
    }else{
        setSpeed(Speed1x);
    }
}


void MainWidget::on_horizontalSlider_valueChanged(int value)
{
    updateValueBySliderValue(value);
}


void MainWidget::valueRender(const QJsonObject& obj)
{
    QJsonDocument doc(obj);
    QString str = QString(doc.toJson(QJsonDocument::Indented));

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


void MainWidget::onIoError(const QString& msg)
{
    emit openFailed();
    emit message(msg);
}


/**
 * @brief MainWidget::on_comboBoxCanIP_currentIndexChanged
 * @param arg1
 */
void MainWidget::on_comboBoxCanIP_currentIndexChanged(const QString& arg1)
{
    Q_UNUSED(arg1);
    int value = ui->horizontalSlider->value();

    updateValueBySliderValue(value);
}
