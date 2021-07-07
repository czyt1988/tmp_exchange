#include "GItemDisplayWidget.h"
#include "ui_GItemDisplayWidget.h"
#include <QPainter>
#include <QDebug>
GItemDisplayWidget::GItemDisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GItemDisplayWidget)
    , m_background(QColor(152, 224, 252))
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


void GItemDisplayWidget::paintEvent(QPaintEvent *e)
{
//    if (!underMouse()) {
//        QPainter p(this);
//        QPen pen(Qt::black);
//        pen.setWidth(1);
//        p.setPen(pen);
//        p.fillRect(rect(), m_background);
//        p.drawRect(rect().adjusted(0, 0, -1, -1));
//    }
    QWidget::paintEvent(e);
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
