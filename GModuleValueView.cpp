#include "GModuleValueView.h"
#include "ui_GModuleValueView.h"
#include <QGridLayout>
GModuleValueView::GModuleValueView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GModuleValueView)
{
    ui->setupUi(this);
    mLayout = new QGridLayout(this);
}


GModuleValueView::~GModuleValueView()
{
    delete ui;
}


void GModuleValueView::append(const GNodeInfo& info)
{
    GItemDisplayWidget *w = new GItemDisplayWidget(this);

    w->setName(info.mName);
    w->setValue(info.mDisplayValue);
    w->setSuffix(info.mSuffix);
    mNodeinfos.append(info);

    int r = (mNodeinfos.size()-1) / 3;
    int c = (mNodeinfos.size()-1) % 3;

    mLayout->addWidget(w, r, c);
    mNodeToWidget[info] = w;
}


void GModuleValueView::setNodeInfos(const QList<GNodeInfo>& infos)
{
    clear();
    for (const GNodeInfo& n : infos)
    {
        append(n);
    }
}


void GModuleValueView::clear()
{
    mNodeinfos.clear();
    mNodeToWidget.clear();
    int c = mLayout->count();

    for (int i = 0; i < c; ++i)
    {
        QLayoutItem *item = mLayout->takeAt(i);
        QWidget *w = item->widget();
        if (w) {
            w->deleteLater();
        }
        delete item;
    }
}


void GModuleValueView::changeEvent(QEvent *e)
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
