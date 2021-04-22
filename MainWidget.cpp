#include "MainWidget.h"
#include "ui_MainWidget.h"
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QScopedPointer>
#include <QDebug>
#include <QFileDialog>
#include "GHvacDataFileIO.h"
MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    mModuleWidget = new GModuleValueView(this);
    ui->scrollArea->setWidget(mModuleWidget);
    ui->splitterSysModule->setStretchFactor(0, 1);
    ui->splitterSysModule->setStretchFactor(1, 5);
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
    GHvacDataFileIO f;

    f.setFileName(QStringLiteral("./start2代.zip"));
    qDebug() << f.open();
}


/**
 * @brief MainWidget::on_pushButtonRun_clicked
 */
void MainWidget::on_pushButtonRun_clicked()
{
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
