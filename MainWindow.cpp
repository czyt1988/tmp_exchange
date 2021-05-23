﻿#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GDataReviewWidget.h"
#include "GTemplate.h"
#include "GTemplateManager.h"
#include <QFileDialog>
#include "GHvacIOManager.h"
#include "SARibbonGalleryGroup.h"
#include "SACustomPlot.h"
const QString c_template_path = "./template";
MainWindow::MainWindow(QWidget *parent) :
    SARibbonMainWindow(parent),
    ui(new UI)
{
    init();
    //默认选中第一个模板
    GTemplateManager::getInstance()->setCurrentTemplate(0);
    SARibbonGalleryGroup *gallgroup = ui->galleryDataTemplate->currentViewGroup();

    if (gallgroup) {
        gallgroup->selectByIndex(0);
    }

    ribbonBar()->setRibbonStyle(SARibbonBar::WpsLiteStyleTwoRow);
    showMaximized();
    ui->figureWidget->resetSplitterRatio();
}


void MainWindow::init()
{
    //加载模板
    GTemplateManager::getInstance()->loadTemplates(c_template_path);
    SARibbonBar *ribbon = ribbonBar();

    ui->tabWidget = new QTabWidget(this);
    ui->tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    ui->tabWidget->setTabPosition(QTabWidget::West);
    this->setCentralWidget(ui->tabWidget);

    ui->statusbar = new QStatusBar(this);
    ui->statusbar->setObjectName(QString::fromUtf8("statusbar"));
    this->setStatusBar(ui->statusbar);
    ui->dockWidgetMessage = new QDockWidget(this);
    ui->dockWidgetMessage->setObjectName(QString::fromUtf8("dockWidgetMessage"));
    ui->dockWidgetContents = new QWidget();
    ui->dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
    ui->verticalLayout = new QVBoxLayout(ui->dockWidgetContents);
    ui->verticalLayout->setSpacing(0);
    ui->verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    ui->textEdit = new QTextEdit(ui->dockWidgetContents);
    ui->textEdit->setObjectName(QString::fromUtf8("textEdit"));

    ui->verticalLayout->addWidget(ui->textEdit);

    ui->dockWidgetMessage->setWidget(ui->dockWidgetContents);
    this->addDockWidget(Qt::RightDockWidgetArea, ui->dockWidgetMessage);

    ui->dockWidgetFault = new QDockWidget(this);
    ui->dockWidgetFault->setObjectName(QString::fromUtf8("dockWidgetFault"));
    ui->dockWidgetContents_2 = new QWidget();
    ui->dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
    ui->verticalLayout_2 = new QVBoxLayout(ui->dockWidgetContents_2);
    ui->verticalLayout_2->setSpacing(0);
    ui->verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    ui->verticalLayout_2->setContentsMargins(0, 0, 0, 0);
    ui->widgetFaule = new GFaultWidget(ui->dockWidgetContents_2);
    ui->widgetFaule->setObjectName(QString::fromUtf8("ui->widgetFaule"));

    ui->verticalLayout_2->addWidget(ui->widgetFaule);

    ui->dockWidgetFault->setWidget(ui->dockWidgetContents_2);
    this->addDockWidget(Qt::RightDockWidgetArea, ui->dockWidgetFault);

    ui->dataReviewWidget = new GDataReviewWidget();
    ui->tabWidget->addTab(ui->dataReviewWidget, QStringLiteral("数据回放"));
    ui->figureWidget = new GPlotWidget();
    ui->tabWidget->addTab(ui->figureWidget, QStringLiteral("数据绘图"));

    //action
    ui->actionOpen = new QAction(this);
    ui->actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
    ui->actionOpen->setIcon(QIcon(":/icon/icon/open.svg"));
    ui->actionDataViewWindow = new QAction(this);
    ui->actionDataViewWindow->setObjectName(QString::fromUtf8("actionDataViewWindow"));
    ui->actionDataViewWindow->setIcon(QIcon(":/icon/icon/dataViewWindow.svg"));
    ui->actionMessageViewWindow = new QAction(this);
    ui->actionMessageViewWindow->setObjectName(QString::fromUtf8("actionMessageViewWindow"));
    ui->actionMessageViewWindow->setIcon(QIcon(":/icon/icon/messageView.svg"));
    ui->actionMessageViewWindow->setCheckable(true);
    ui->actionFaultViewWindow = new QAction(this);
    ui->actionFaultViewWindow->setObjectName(QString::fromUtf8("actionFaultViewWindow"));
    ui->actionFaultViewWindow->setIcon(QIcon(":/icon/icon/faultView.svg"));
    ui->actionFaultViewWindow->setCheckable(true);
    ui->actionFigureWindow = new QAction(this);
    ui->actionFigureWindow->setObjectName(QString::fromUtf8("actionFigureWindow"));
    ui->actionFigureWindow->setIcon(QIcon(":/icon/icon/figureWindow.svg"));
    ui->actionRunOrStopDataView = new QAction(this);
    ui->actionRunOrStopDataView->setObjectName(QString::fromUtf8("actionRunOrStopDataView"));
    ui->actionRunOrStopDataView->setIcon(QIcon(":/icon/icon/start.svg"));
    ui->actionRunDataViewSpeed1 = new QAction(this);
    ui->actionRunDataViewSpeed1->setObjectName(QString::fromUtf8("actionRunDataViewSpeed1"));
    ui->actionRunDataViewSpeed1->setIcon(QIcon(":/icon/icon/1x.svg"));
    ui->actionRunDataViewSpeed1->setCheckable(true);
    ui->actionRunDataViewSpeed2 = new QAction(this);
    ui->actionRunDataViewSpeed2->setObjectName(QString::fromUtf8("actionRunDataViewSpeed2"));
    ui->actionRunDataViewSpeed2->setIcon(QIcon(":/icon/icon/2x.svg"));
    ui->actionRunDataViewSpeed2->setCheckable(true);
    ui->actionRunDataViewSpeed3 = new QAction(this);
    ui->actionRunDataViewSpeed3->setObjectName(QString::fromUtf8("actionRunDataViewSpeed3"));
    ui->actionRunDataViewSpeed3->setIcon(QIcon(":/icon/icon/3x.svg"));
    ui->actionRunDataViewSpeed3->setCheckable(true);
    ui->actionRunDataViewSpeedMax = new QAction(this);
    ui->actionRunDataViewSpeedMax->setObjectName(QString::fromUtf8("actionRunDataViewSpeedMax"));
    ui->actionRunDataViewSpeedMax->setIcon(QIcon(":/icon/icon/maxSpeed.svg"));
    ui->actionRunDataViewSpeedMax->setCheckable(true);
    ui->actionGroupRunDataViewSpeed = new QActionGroup(this);
    ui->actionGroupRunDataViewSpeed->addAction(ui->actionRunDataViewSpeed1);
    ui->actionGroupRunDataViewSpeed->addAction(ui->actionRunDataViewSpeed2);
    ui->actionGroupRunDataViewSpeed->addAction(ui->actionRunDataViewSpeed3);
    ui->actionGroupRunDataViewSpeed->addAction(ui->actionRunDataViewSpeedMax);
    ui->actionGroupRunDataViewSpeed->setExclusive(true);
    ui->actionRunDataViewSpeed1->setChecked(true);
    ui->actionFigureLegend = new QAction(this);
    ui->actionFigureLegend->setObjectName(QString::fromUtf8("actionFigureLegend"));
    ui->actionFigureLegend->setIcon(QIcon(":/icon/icon/chartLegend.svg"));
    ui->actionFigureLegend->setCheckable(true);
    ui->actionFigureWheelZoomable = new QAction(this);
    ui->actionFigureWheelZoomable->setObjectName(QString::fromUtf8("actionFigureZoomable"));
    ui->actionFigureWheelZoomable->setIcon(QIcon(":/icon/icon/chartZoom.svg"));
    ui->actionFigureWheelZoomable->setCheckable(true);
    ui->actionFigureRangeDrag = new QAction(this);
    ui->actionFigureRangeDrag->setObjectName(QString::fromUtf8("actionFigureRangeDrag"));
    ui->actionFigureRangeDrag->setIcon(QIcon(":/icon/icon/chartRangeDrag.svg"));
    ui->actionFigureRangeDrag->setCheckable(true);
    ui->actionFigureAxesSelect = new QAction(this);
    ui->actionFigureAxesSelect->setObjectName(QString::fromUtf8("actionFigureAxesSelect"));
    ui->actionFigureAxesSelect->setIcon(QIcon(":/icon/icon/chartAxesSelect.svg"));
    ui->actionFigureAxesSelect->setCheckable(true);
    ui->actionFigureLegendSelect = new QAction(this);
    ui->actionFigureLegendSelect->setObjectName(QString::fromUtf8("actionFigureLegendSelect"));
    ui->actionFigureLegendSelect->setIcon(QIcon(":/icon/icon/chartLegendSelect.svg"));
    ui->actionFigureLegendSelect->setCheckable(true);
    ui->actionFigureRectSelectZoom = new QAction(this);
    ui->actionFigureRectSelectZoom->setObjectName(QString::fromUtf8("actionFigureRectSelectZoom"));
    ui->actionFigureRectSelectZoom->setIcon(QIcon(":/icon/icon/chartSelectZoom.svg"));
    ui->actionFigureRectSelectZoom->setCheckable(true);
    //建立ribbon
    //categoryMain
    ui->categoryMain = new SARibbonCategory();
    ribbon->addCategoryPage(ui->categoryMain);

    ui->pannelMainFile = new SARibbonPannel();
    ui->pannelMainFile->addLargeAction(ui->actionOpen);
    ui->categoryMain->addPannel(ui->pannelMainFile);

    ui->pannelMainWindowList = new SARibbonPannel();
    ui->pannelMainWindowList->addLargeAction(ui->actionDataViewWindow);
    ui->pannelMainWindowList->addLargeAction(ui->actionFigureWindow);
    ui->pannelMainWindowList->addSmallAction(ui->actionMessageViewWindow);
    ui->pannelMainWindowList->addSmallAction(ui->actionFaultViewWindow);
    ui->categoryMain->addPannel(ui->pannelMainWindowList);

    ui->pannelMainDataTemplate = new SARibbonPannel();
    ui->galleryDataTemplate = new SARibbonGallery(ui->pannelMainDataTemplate);
    QList<GTemplate *> temps = GTemplateManager::getInstance()->getAllTemplates();

    for (GTemplate *t : temps)
    {
        QAction *a = new QAction(this);
        a->setIcon(QIcon(":/icon/icon/template.svg"));
        a->setText(t->name());
        ui->templateActionList.append(a);
    }
    ui->galleryDataTemplate->addCategoryActions(QStringLiteral("模板"), ui->templateActionList);
    ui->pannelMainDataTemplate->addWidget(ui->galleryDataTemplate, SARibbonPannelItem::Large);
    ui->pannelMainDataTemplate->setExpanding();
    ui->categoryMain->addPannel(ui->pannelMainDataTemplate);
    //categoryDataView
    ui->categoryDataView = new SARibbonCategory();
    ribbon->addCategoryPage(ui->categoryDataView);

    ui->pannelDataViewOpetion = new SARibbonPannel();
    ui->pannelDataViewOpetion->addLargeAction(ui->actionRunOrStopDataView);
    ui->pannelDataViewOpetion->addSmallAction(ui->actionRunDataViewSpeed1);
    ui->pannelDataViewOpetion->addSmallAction(ui->actionRunDataViewSpeed2);
    ui->pannelDataViewOpetion->addSmallAction(ui->actionRunDataViewSpeed3);
    ui->pannelDataViewOpetion->addSmallAction(ui->actionRunDataViewSpeedMax);
    ui->categoryDataView->addPannel(ui->pannelDataViewOpetion);
    //categoryFigure
    ui->categoryFigure = new SARibbonCategory(this);
    ribbon->addCategoryPage(ui->categoryFigure);
    ui->pannelFigureOpetion = new SARibbonPannel(this);
    ui->pannelFigureOpetion->addLargeAction(ui->actionFigureLegend);
    ui->pannelFigureOpetion->addLargeAction(ui->actionFigureRectSelectZoom);
    ui->pannelFigureOpetion->addLargeAction(ui->actionFigureWheelZoomable);
    ui->pannelFigureOpetion->addLargeAction(ui->actionFigureRangeDrag);
    ui->pannelFigureOpetion->addLargeAction(ui->actionFigureAxesSelect);
    ui->pannelFigureOpetion->addLargeAction(ui->actionFigureLegendSelect);

    ui->categoryFigure->addPannel(ui->pannelFigureOpetion);
    //组建立ribbon界面

    //某些状态的初始化
    ui->actionMessageViewWindow->setChecked(true);
    ui->actionFaultViewWindow->setChecked(true);
    ui->actionFigureLegend->setChecked(ui->figureWidget->figure()->isLegendVisible());
    ui->actionFigureWheelZoomable->setChecked(ui->figureWidget->figure()->isEnableWheelZoom());
    ui->actionFigureRangeDrag->setChecked(ui->figureWidget->figure()->isEnableRangeDrag());
    ui->actionFigureAxesSelect->setChecked(ui->figureWidget->figure()->isEnableAxesSelect());
    ui->actionFigureLegendSelect->setChecked(ui->figureWidget->figure()->isEnableLegendSelect());
    ui->actionFigureRectSelectZoom->setChecked(ui->figureWidget->figure()->isEnableSelectRectZoom());
    //初始化
    connect(IOManager, &GHvacIOManager::startOpenFile, this, &MainWindow::onOpenFile);
    connect(IOManager, &GHvacIOManager::fileReaded, this, &MainWindow::onFileReaded);
    connect(IOManager, &GHvacIOManager::message, this, &MainWindow::onMessage);
    connect(IOManager, &GHvacIOManager::openFailed, this, &MainWindow::onOpenFailed);
    connect(TemplateManager, &GTemplateManager::templateChanged, this, &MainWindow::onTemplateChanged);
    connect(ui->dataReviewWidget, &GDataReviewWidget::message, this, &MainWindow::onMessage);
    connect(ui->widgetFaule, &GFaultWidget::indexReques, ui->dataReviewWidget, &GDataReviewWidget::toIndex);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpenTriggered);
    connect(ui->actionDataViewWindow, &QAction::triggered, this, &MainWindow::onActionDataViewWindowTriggered);
    connect(ui->actionMessageViewWindow, &QAction::triggered, this, &MainWindow::onActionMessageViewWindowTriggered);
    connect(ui->actionFaultViewWindow, &QAction::triggered, this, &MainWindow::onActionFaultViewWindowTriggered);
    connect(ui->actionFigureWindow, &QAction::triggered, this, &MainWindow::onActionFigureWindowTriggered);
    connect(ui->actionRunOrStopDataView, &QAction::triggered, this, &MainWindow::onActionRunOrStopDataViewTriggered);
    connect(ui->actionGroupRunDataViewSpeed, &QActionGroup::triggered, this, &MainWindow::onActionGroupRunDataViewSpeedTriggered);
    connect(ui->galleryDataTemplate, &SARibbonGallery::triggered, this, &MainWindow::onGalleryTemplateActionTriggered);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabWidgetCurrentChanged);
    connect(ui->actionFigureLegend, &QAction::triggered, this, &MainWindow::onActionFigureLegendTriggered);
    connect(ui->actionFigureWheelZoomable, &QAction::triggered, this, &MainWindow::onActionFigureWheelZoomableTriggered);
    connect(ui->actionFigureRectSelectZoom, &QAction::triggered, this, &MainWindow::onActionFigureRectSelectZoomTriggered);
    connect(ui->actionFigureRangeDrag, &QAction::triggered, this, &MainWindow::onActionFigureRangeDragTriggered);
    connect(ui->actionFigureAxesSelect, &QAction::triggered, this, &MainWindow::onActionFigureAxesSelectTriggered);
    connect(ui->actionFigureLegendSelect, &QAction::triggered, this, &MainWindow::onActionFigureLegendSelectTriggered);
    //文本设置
    ui->retranslateUi(this);
}


void MainWindow::UI::retranslateUi(MainWindow *w)
{
    w->setWindowTitle(QStringLiteral("格力大数据工作台"));
    dockWidgetMessage->setWindowTitle(QStringLiteral("消息"));
    dockWidgetFault->setWindowTitle(QStringLiteral("故障信息"));
    w->ribbonBar()->applicationButton()->setText(QStringLiteral("GREE"));
    categoryMain->setCategoryName(QStringLiteral("主页"));
    categoryDataView->setCategoryName(QStringLiteral("回放"));
    categoryFigure->setCategoryName(QStringLiteral("绘图"));
    pannelMainFile->setPannelName(QStringLiteral("文件"));
    pannelMainWindowList->setPannelName(QStringLiteral("窗口"));
    pannelMainDataTemplate->setPannelName(QStringLiteral("数据模板"));
    pannelDataViewOpetion->setPannelName(QStringLiteral("回放操作"));
    pannelFigureOpetion->setPannelName(QStringLiteral("绘图操作"));
    //action
    actionOpen->setText(QStringLiteral("打开"));
    actionDataViewWindow->setText(QStringLiteral("回放视图"));
    actionFigureWindow->setText(QStringLiteral("绘图视图"));
    actionMessageViewWindow->setText(QStringLiteral("消息窗口"));
    actionFaultViewWindow->setText(QStringLiteral("故障窗口"));
    actionRunOrStopDataView->setText(QStringLiteral("运行"));
    actionRunDataViewSpeed1->setText(QStringLiteral("1x播放速度"));
    actionRunDataViewSpeed2->setText(QStringLiteral("2x播放速度"));
    actionRunDataViewSpeed3->setText(QStringLiteral("3x播放速度"));
    actionRunDataViewSpeedMax->setText(QStringLiteral("最大播放速度"));
    actionFigureLegend->setText(QStringLiteral("图例"));
    actionFigureWheelZoomable->setText(QStringLiteral("滚轮缩放"));
    actionFigureRectSelectZoom->setText(QStringLiteral("缩放"));
    actionFigureRangeDrag->setText(QStringLiteral("拖动"));
    actionFigureAxesSelect->setText(QStringLiteral("坐标轴拖动"));
    actionFigureLegendSelect->setText(QStringLiteral("图例可选择"));
}


MainWindow::~MainWindow()
{
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

    default:
        break;
    }
}


void MainWindow::onMessage(const QString& msg)
{
    ui->textEdit->append(msg);
}


void MainWindow::onFileReaded(GHvacDataInfo data)
{
    ui->actionOpen->setEnabled(true);
    ui->actionRunOrStopDataView->setEnabled(true);
    //
    ui->dataReviewWidget->setData(data);
    //给故障窗口传递数据
    ui->widgetFaule->setData(data);
//    ui->widgetFaule->setTemplate(ui->dataReviewWidget->getCurrentTemplate());
    ui->widgetFaule->updateFaultInfo();
    //给绘图窗口传递数据
    ui->figureWidget->setData(data);
//    ui->dataPlotWidget->setTemplate(ui->dataReviewWidget->getCurrentTemplate());
    //显示标签
    ribbonBar()->showCategory(ui->categoryDataView);
}


void MainWindow::onTemplateChanged(GTemplate *temp)
{
    ui->dataReviewWidget->setTemplate(temp);
    ui->widgetFaule->setTemplate(temp);
    ui->widgetFaule->updateFaultInfo();
    ui->figureWidget->setTemplate(temp);
}


void MainWindow::onActionFigureWindowTriggered()
{
    ui->tabWidget->setCurrentWidget(ui->figureWidget);
}


void MainWindow::onActionMessageViewWindowTriggered(bool c)
{
    if (c) {
        ui->dockWidgetMessage->show();
        ui->dockWidgetMessage->raise();
    }else{
        ui->dockWidgetMessage->hide();
    }
}


void MainWindow::onActionFaultViewWindowTriggered(bool c)
{
    if (c) {
        ui->dockWidgetMessage->show();
        ui->dockWidgetMessage->raise();
    }else{
        ui->dockWidgetMessage->hide();
    }
}


/**
 * @brief 正打开文件
 * @param filename
 */
void MainWindow::onOpenFile(const QString& filename)
{
    ui->statusbar->showMessage(filename);
    ui->dataReviewWidget->startOpenReviewData();
}


void MainWindow::onOpenFailed()
{
    ui->actionOpen->setEnabled(true);
    ui->actionRunOrStopDataView->setEnabled(true);
}


/**
 * @brief 运行数据回放
 */
void MainWindow::onActionRunOrStopDataViewTriggered()
{
    if ((ui->dataReviewWidget->getMode() == GDataReviewWidget::StoppedMode) ||
        (ui->dataReviewWidget->getMode() == GDataReviewWidget::NoneMode)) {
        ui->dataReviewWidget->setRunMode();
        ui->actionRunOrStopDataView->setIcon(QIcon(":/icon/icon/pause.svg"));
        ui->actionRunOrStopDataView->setText(QStringLiteral("暂停"));
    }else if (ui->dataReviewWidget->getMode() == GDataReviewWidget::RuningMode) {
        ui->dataReviewWidget->setStopMode();
        ui->actionRunOrStopDataView->setIcon(QIcon(":/icon/icon/start.svg"));
        ui->actionRunOrStopDataView->setText(QStringLiteral("开始"));
    }
}


void MainWindow::onActionGroupRunDataViewSpeedTriggered(QAction *action)
{
    if (action == ui->actionRunDataViewSpeed1) {
        ui->dataReviewWidget->setSpeed(GDataReviewWidget::Speed1x);
    }else if (action == ui->actionRunDataViewSpeed2) {
        ui->dataReviewWidget->setSpeed(GDataReviewWidget::Speed2x);
    }else if (action == ui->actionRunDataViewSpeed3) {
        ui->dataReviewWidget->setSpeed(GDataReviewWidget::Speed3x);
    }else if (action == ui->actionRunDataViewSpeedMax) {
        ui->dataReviewWidget->setSpeed(GDataReviewWidget::SpeedMax);
    }
}


/**
 * @brief 模板gallery点击触发
 * @param action
 */
void MainWindow::onGalleryTemplateActionTriggered(QAction *action)
{
    int index = ui->templateActionList.indexOf(action);

    GTemplateManager::getInstance()->setCurrentTemplate(index);
}


/**
 * @brief tab标签选中后，ribbon category随之跳转
 *
 * 前提是已经读取数据
 * @param index
 */
void MainWindow::onTabWidgetCurrentChanged(int index)
{
    if (!IOManager->isHaveData()) {
        return;
    }
    if (ui->tabWidget->widget(index) == ui->dataReviewWidget) {
        ribbonBar()->showCategory(ui->categoryDataView);
    }else if (ui->tabWidget->widget(index) == ui->figureWidget) {
        ribbonBar()->showCategory(ui->categoryFigure);
    }
}


/**
 * @brief 图例
 * @param c
 */
void MainWindow::onActionFigureLegendTriggered(bool c)
{
    ui->figureWidget->figure()->showLegend(c);
}


/**
 * @brief 滚轮缩放
 * @param c
 */
void MainWindow::onActionFigureWheelZoomableTriggered(bool c)
{
    ui->figureWidget->figure()->enableWheelZoom(c);
}


/**
 * @brief 选框缩放
 * @param c
 */
void MainWindow::onActionFigureRectSelectZoomTriggered(bool c)
{
    //选框缩放和拖动不能共存
    ui->figureWidget->figure()->enableSelectRectZoom(c);
    if (ui->figureWidget->figure()->isEnableRangeDrag()) {
        //选框和拖动不能共存
        ui->figureWidget->figure()->enableRangeDrag(false);
        ui->actionFigureRangeDrag->setChecked(false);
    }
}


/**
 * @brief 拖动控制
 * @param c
 */
void MainWindow::onActionFigureRangeDragTriggered(bool c)
{
    ui->figureWidget->figure()->enableRangeDrag(c);
    if (ui->figureWidget->figure()->isEnableSelectRectZoom()) {
        //选框和拖动不能共存
        ui->figureWidget->figure()->enableSelectRectZoom(false);
        ui->actionFigureRectSelectZoom->setChecked(false);
    }
}


/**
 * @brief 允许坐标轴可选
 * @param c
 */
void MainWindow::onActionFigureAxesSelectTriggered(bool c)
{
    ui->figureWidget->figure()->enableAxesSelect(c);
}


/**
 * @brief 允许Legend可选
 * @param c
 */
void MainWindow::onActionFigureLegendSelectTriggered(bool c)
{
    ui->figureWidget->figure()->enableLegendSelect(c);
}


void MainWindow::onActionOpenTriggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, QStringLiteral("选择数据文件"), "", "数据文件 (*.zip)");

    if (filepath.isEmpty()) {
        return;
    }
    IOManager->openReviewData(filepath);
    ui->actionOpen->setDisabled(true);
    ui->actionRunOrStopDataView->setDisabled(true);
}


/**
 * @brief 回放视图
 */
void MainWindow::onActionDataViewWindowTriggered()
{
    ui->tabWidget->setCurrentWidget(ui->dataReviewWidget);
}


void MainWindow::onFaultWidgetIndexReques(int index)
{
}
