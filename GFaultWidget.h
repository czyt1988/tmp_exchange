#ifndef GFAULTWIDGET_H
#define GFAULTWIDGET_H

#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include "GNodeInfo.h"
#include "GHvacDataInfo.h"

namespace Ui {
class GFaultWidget;
}

class GFaultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GFaultWidget(QWidget *parent = 0);
    ~GFaultWidget();
    void ininModel();
    void updateFaultInfo(const QList<GNodeInfo>& sys, const QList<GNodeInfo>& module, const QList<GNodeInfo>& idu, const GHvacDataInfo& res);
    void findFault(const GNodeInfo& info);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GFaultWidget *ui;
    QStandardItemModel *m_model;
    QList<GNodeInfo> mSystemFaultSrc;
    QList<GNodeInfo> mModuleFaultSrc;
    QList<GNodeInfo> mIduFaultSrc;
    GHvacDataInfo m_resInfo;
};

#endif // GFAULTWIDGET_H
