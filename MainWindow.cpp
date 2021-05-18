#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MainWidget.h"
#include "GTemplate.h"

MainWindow::MainWindow(QWidget *parent) :
    SARibbonMainWindow(parent),
    ui(new UI)
{
    init();
    ribbonBar()->setRibbonStyle(SARibbonBar::WpsLiteStyleTwoRow);
    showMaximized();
}


void MainWindow::init()
{
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

    ui->dataReviewWidget = new MainWidget();
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
    //建立ribbon

    ui->categoryMain = new SARibbonCategory();
    ribbon->addCategoryPage(ui->categoryMain);

    ui->main_filePannel = new SARibbonPannel();
    ui->main_filePannel->addLargeAction(ui->actionOpen);
    ui->categoryMain->addPannel(ui->main_filePannel);

    ui->main_WindowPannel = new SARibbonPannel();
    ui->main_WindowPannel->addLargeAction(ui->actionDataViewWindow);
    ui->main_WindowPannel->addLargeAction(ui->actionFigureWindow);
    ui->categoryMain->addPannel(ui->main_WindowPannel);

    ui->categoryDataView = new SARibbonCategory();
    ribbon->addCategoryPage(ui->categoryDataView);

    ui->categoryFigure = new SARibbonCategory();
    ribbon->addCategoryPage(ui->categoryFigure);

    //组建立ribbon界面


    //信号槽
    connect(ui->dataReviewWidget, &MainWidget::message, this, &MainWindow::onMessage);
    connect(ui->dataReviewWidget, &MainWidget::fileReaded, this, &MainWindow::onFileReaded);
    connect(ui->dataReviewWidget, &MainWidget::templateChanged, this, &MainWindow::onTemplateChanged);
    connect(ui->dataReviewWidget, &MainWidget::openFile, this, &MainWindow::onOpenFile);
    connect(ui->dataReviewWidget, &MainWidget::openFailed, this, &MainWindow::onOpenFailed);
    connect(ui->widgetFaule, &GFaultWidget::indexReques, ui->dataReviewWidget, &MainWidget::toIndex);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpenTriggered);
    connect(ui->actionDataViewWindow, &QAction::triggered, this, &MainWindow::onActionDataViewWindowTriggered);
    connect(ui->actionFigureWindow, &QAction::triggered, this, &MainWindow::onActionFigureWindowTriggered);
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
    main_filePannel->setPannelName(QStringLiteral("文件"));
    main_WindowPannel->setPannelName(QStringLiteral("窗口"));
    //action
    actionOpen->setText(QStringLiteral("打开"));
    actionDataViewWindow->setText(QStringLiteral("回放视图"));
    actionFigureWindow->setText(QStringLiteral("绘图视图"));
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
    //给故障窗口传递数据
    ui->widgetFaule->setData(data);
    ui->widgetFaule->setTemplate(ui->dataReviewWidget->getCurrentTemplate());
    ui->widgetFaule->updateFaultInfo();
    //给绘图窗口传递数据
    ui->dataPlotWidget->setData(data);
    ui->dataPlotWidget->setTemplate(ui->dataReviewWidget->getCurrentTemplate());
    //显示标签
    ribbonBar()->showCategory(ui->categoryDataView);
}


void MainWindow::onTemplateChanged(GTemplate *temp)
{
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
}


void MainWindow::onOpenFailed()
{
    ui->actionOpen->setEnabled(true);
}


void MainWindow::onActionOpenTriggered()
{
    ui->dataReviewWidget->open();
    ui->actionOpen->setDisabled(true);
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
