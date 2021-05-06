#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MainWidget.h"
#include "GTemplate.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mMainWidget = new MainWidget(this);
    setCentralWidget(mMainWidget);
    connect(mMainWidget, &MainWidget::message, this, &MainWindow::onMessage);
    connect(mMainWidget, &MainWidget::fileReaded, this, &MainWindow::onFileReaded);
    connect(mMainWidget, &MainWidget::templateChanged, this, &MainWindow::onTemplateChanged);
    connect(ui->widgetFaule, &GFaultWidget::indexReques, mMainWidget, &MainWidget::toIndex);
    showMaximized();
}


MainWindow::~MainWindow()
{
    delete ui;
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
    ui->widgetFaule->setData(data);
    ui->widgetFaule->setTemplate(mMainWidget->getCurrentTemplate());
    ui->widgetFaule->updateFaultInfo();
}


void MainWindow::onTemplateChanged(GTemplate *temp)
{
    ui->widgetFaule->setTemplate(temp);
    ui->widgetFaule->updateFaultInfo();
}


void MainWindow::onFaultWidgetIndexReques(int index)
{
}
