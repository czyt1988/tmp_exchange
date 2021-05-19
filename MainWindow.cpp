#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GDataReviewWidget.h"
#include "GTemplate.h"
#include "GTemplateManager.h"
#include <QFileDialog>
#include "GHvacIOManager.h"
const QString c_template_path = "./template";
MainWindow::MainWindow(QWidget *parent) :
    SARibbonMainWindow(parent),
    ui(new UI)
{
    init();
    GTemplateManager::getInstance()->setCurrentTemplate(0);
    ribbonBar()->setRibbonStyle(SARibbonBar::WpsLiteStyleTwoRow);
    showMaximized();
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
    ui->dataPlotWidget = new GPlotWidget();
    ui->tabWidget->addTab(ui->dataPlotWidget, QStringLiteral("数据绘图"));

    //action
    ui->actionOpen = new QAction(this);
    ui->actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
    ui->actionOpen->setIcon(QIcon(":/icon/icon/open.svg"));
    ui->actionDataViewWindow = new QAction(this);
    ui->actionDataViewWindow->setObjectName(QString::fromUtf8("actionDataViewWindow"));
    ui->actionDataViewWindow->setIcon(QIcon(":/icon/icon/dataViewWindow.svg"));
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

    ui->dataview_opetion = new SARibbonPannel();
    ui->dataview_opetion->addLargeAction(ui->actionRunOrStopDataView);
    ui->dataview_opetion->addSmallAction(ui->actionRunDataViewSpeed1);
    ui->dataview_opetion->addSmallAction(ui->actionRunDataViewSpeed2);
    ui->dataview_opetion->addSmallAction(ui->actionRunDataViewSpeed3);
    ui->dataview_opetion->addSmallAction(ui->actionRunDataViewSpeedMax);
    ui->categoryDataView->addPannel(ui->dataview_opetion);
    //categoryFigure
    ui->categoryFigure = new SARibbonCategory();
    ribbon->addCategoryPage(ui->categoryFigure);

    //组建立ribbon界面


    //信号槽
    connect(IOManager, &GHvacIOManager::startOpenFile, this, &MainWindow::onOpenFile);
    connect(IOManager, &GHvacIOManager::fileReaded, this, &MainWindow::onFileReaded);
    connect(IOManager, &GHvacIOManager::message, this, &MainWindow::onMessage);
    connect(IOManager, &GHvacIOManager::openFailed, this, &MainWindow::onOpenFailed);
    connect(TemplateManager, &GTemplateManager::templateChanged, this, &MainWindow::onTemplateChanged);
    connect(ui->dataReviewWidget, &GDataReviewWidget::message, this, &MainWindow::onMessage);
    connect(ui->widgetFaule, &GFaultWidget::indexReques, ui->dataReviewWidget, &GDataReviewWidget::toIndex);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpenTriggered);
    connect(ui->actionDataViewWindow, &QAction::triggered, this, &MainWindow::onActionDataViewWindowTriggered);
    connect(ui->actionFigureWindow, &QAction::triggered, this, &MainWindow::onActionFigureWindowTriggered);
    connect(ui->actionRunOrStopDataView, &QAction::triggered, this, &MainWindow::onActionRunOrStopDataViewTriggered);
    connect(ui->actionGroupRunDataViewSpeed, &QActionGroup::triggered, this, &MainWindow::onActionGroupRunDataViewSpeedTriggered);
    connect(ui->galleryDataTemplate, &SARibbonGallery::triggered, this, &MainWindow::onGalleryTemplateActionTriggered);
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
    dataview_opetion->setPannelName(QStringLiteral("操作"));
    //action
    actionOpen->setText(QStringLiteral("打开"));
    actionDataViewWindow->setText(QStringLiteral("回放视图"));
    actionFigureWindow->setText(QStringLiteral("绘图视图"));
    actionRunOrStopDataView->setText(QStringLiteral("运行"));
    actionRunDataViewSpeed1->setText(QStringLiteral("1x播放速度"));
    actionRunDataViewSpeed2->setText(QStringLiteral("2x播放速度"));
    actionRunDataViewSpeed3->setText(QStringLiteral("3x播放速度"));
    actionRunDataViewSpeedMax->setText(QStringLiteral("最大播放速度"));
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
    ui->dataPlotWidget->setData(data);
//    ui->dataPlotWidget->setTemplate(ui->dataReviewWidget->getCurrentTemplate());
    //显示标签
    ribbonBar()->showCategory(ui->categoryDataView);
}


void MainWindow::onTemplateChanged(GTemplate *temp)
{
    ui->dataReviewWidget->setTemplate(temp);
    ui->widgetFaule->setTemplate(temp);
    ui->widgetFaule->updateFaultInfo();
    ui->dataPlotWidget->setTemplate(temp);
}


void MainWindow::onActionFigureWindowTriggered()
{
    ui->tabWidget->setCurrentWidget(ui->dataPlotWidget);
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
