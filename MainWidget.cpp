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
{
    ui->setupUi(this);
    connect(&mTimer, &QTimer::timeout, this, &MainWidget::onTimeout);

    mModuleWidget = new GModuleValueView(this);
    ui->scrollArea->setWidget(mModuleWidget);
    ui->splitterSysModule->setStretchFactor(0, 1);
    ui->splitterSysModule->setStretchFactor(1, 5);
    setStopMode();
    setSpeed(Speed1x);
    loadTemplate();
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


void MainWidget::loadTemplate()
{
    deleteTemplates();
    QDir dir("./template");
    QStringList xmlfilelist = dir.entryList(QDir::NoDotAndDotDot|QDir::Files);

    for (const QString& p : xmlfilelist)
    {
        qDebug() << dir.path() + QDir::separator() + p;
        QScopedPointer<GTemplate> tp(new GTemplate(this));
        if (tp->load(dir.path() + QDir::separator() + p)) {
            mTemplate.append(tp.take());
        }
    }
    for (GTemplate *t : mTemplate)
    {
        ui->comboBoxTemplate->addItem(t->name());
    }
    if (ui->comboBoxTemplate->count() > 0) {
        ui->comboBoxTemplate->setCurrentIndex(0);
    }
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


/**
 * @brief MainWidget::on_pushButtonBrower_clicked
 */
void MainWidget::on_pushButtonBrower_clicked()
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
    ui->lineEdit->setText(filepath);
    ui->pushButtonBrower->setDisabled(true);
    ui->pushButtonRun->setDisabled(true);
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


/**
 * @brief MainWidget::on_pushButtonRun_clicked
 */
void MainWidget::on_pushButtonRun_clicked()
{
    if ((mMode == StoppedMode) || (mMode == NoneMode)) {
        setRunMode();
    }else if (mMode == RuningMode) {
        setStopMode();
    }
}


void MainWidget::on_comboBoxTemplate_currentIndexChanged(int index)
{
    if (index >= mTemplate.size()) {
        qDebug() << QStringLiteral("combox索引异常");
        return;
    }
    GTemplate *t = mTemplate.at(index);

    setTemplate(t);
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
        ui->pushButtonBrower->setEnabled(true);
        ui->pushButtonRun->setEnabled(true);
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

    ui->pushButtonBrower->setEnabled(true);
    ui->pushButtonRun->setEnabled(true);
}


void MainWidget::onIOErrRec(const QString& msg)
{
    ui->pushButtonBrower->setEnabled(true);
    ui->pushButtonRun->setEnabled(true);
    emit message(msg);
}


void MainWidget::onTimeout()
{
    int v = ui->horizontalSlider->value();
    int s = ui->horizontalSlider->singleStep();

    ui->horizontalSlider->setValue(v+s);
}


void MainWidget::updateValue(int toSecsSinceEpoch)
{
    //首先更新系统数据
    QJsonObject json = mHvacInfo.get(toSecsSinceEpoch);

    valueRender(json);
}


/**
 * @brief 删除所有模板
 */
void MainWidget::deleteTemplates()
{
    for (auto i : mTemplate)
    {
        i->deleteLater();
    }
    mCurrentTemplate = nullptr;
    mTemplate.clear();
    ui->comboBoxTemplate->clear();
}


void MainWidget::setRunMode()
{
    ui->pushButtonRun->setIcon(QIcon(":/icon/icon/pause.svg"));
    mMode = RuningMode;
    mTimer.start();
}


void MainWidget::setStopMode()
{
    ui->pushButtonRun->setIcon(QIcon(":/icon/icon/start.svg"));
    mMode = StoppedMode;
    mTimer.stop();
}


void MainWidget::setNoneMode()
{
    ui->pushButtonRun->setIcon(QIcon(":/icon/icon/start.svg"));
    ui->pushButtonRun->setDisabled(true);
    mMode = NoneMode;
}


void MainWidget::setSpeed(MainWidget::Speed s)
{
    mCurrentSpeed = s;
    switch (s)
    {
    case Speed1x:
    {
        QPixmap p(":/icon/icon/1x.svg");
        p = p.scaledToHeight(ui->labelspeed->height());
        ui->labelspeed->setPixmap(p);
        mTimer.setInterval(1000);
    }
    break;

    case Speed2x:
    {
        QPixmap p(":/icon/icon/2x.svg");
        p = p.scaledToHeight(ui->labelspeed->height());
        ui->labelspeed->setPixmap(p);
        mTimer.setInterval(500);
    }
    break;

    case Speed3x:
    {
        QPixmap p(":/icon/icon/3x.svg");
        p = p.scaledToHeight(ui->labelspeed->height());
        ui->labelspeed->setPixmap(p);
        mTimer.setInterval(300);
    }
    break;

    default:
        break;
    }
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
    if ((value >= mHvacInfo.allDateTimeScale.size()) || (value < 0)) {
        qDebug() << "value out range";
        if (mMode == RuningMode) {
            setStopMode();
        }
        return;
    }
    QDateTime dt = mHvacInfo.allDateTimeScale.at(value);

    int secsSinceEpoch = dt.toSecsSinceEpoch();

    ui->labelCurrentTime->setText(QStringLiteral("当前时间:%1").arg(dt.toString("yyyy-MM-dd HH:mm:ss")));
    updateValue(secsSinceEpoch);
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
        i = sys.find(n.mSrc);
        if (i == sys.constEnd()) {
            qDebug() << QStringLiteral("无法在system中找到参数key:") << n.mSrc;
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
            qDebug() << QStringLiteral("无法在module中找到参数key:") << n.mSrc;
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
            i = iduwithip.find(n.mSrc);
            //所有canip遍历一遍
            if (i == iduwithip.constEnd()) {
                qDebug() << QStringLiteral("无法在%1中找到参数key:").arg(kname) << n.mSrc;
                continue;
            }
            n.mDisplayValue = (*i).toString();
        }
        mCurrentTemplate->getIduModel()->updateValue(idunodes);
    }
}


void MainWidget::onIoError(const QString& msg)
{
    ui->pushButtonBrower->setEnabled(true);
    ui->pushButtonRun->setEnabled(true);
    emit message(msg);
}
