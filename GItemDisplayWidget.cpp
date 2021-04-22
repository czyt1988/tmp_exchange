#include "GItemDisplayWidget.h"
#include "ui_GItemDisplayWidget.h"

GItemDisplayWidget::GItemDisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GItemDisplayWidget)
{
    ui->setupUi(this);
}


GItemDisplayWidget::~GItemDisplayWidget()
{
    delete ui;
}


void GItemDisplayWidget::setValue(const QString& value)
{
    ui->labelValue->setText(value);
}


void GItemDisplayWidget::setName(const QString& name)
{
    ui->labelTitle->setText(name);
}


void GItemDisplayWidget::setSuffix(const QString& sf)
{
    ui->labelSuffix->setText(sf);
}


void GItemDisplayWidget::changeEvent(QEvent *e)
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
