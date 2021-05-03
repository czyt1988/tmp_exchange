#ifndef GFAULTWIDGET_H
#define GFAULTWIDGET_H

#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include "GNodeInfo.h"
#include "GHvacDataInfo.h"
#include "GTemplate.h"
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
    void setTemplate(GTemplate *temp);
    void setData(const GHvacDataInfo& res);
    void updateFaultInfo();

protected:
    int findFault(const GNodeInfo& info, GHvacDataInfo::TablePtr tp, const QIcon& icon = QIcon());
    void changeEvent(QEvent *e);

private:
    Ui::GFaultWidget *ui;
    QStandardItemModel *m_model;
    GHvacDataInfo m_resInfo;
    GTemplate *mTemplate;
};

#endif // GFAULTWIDGET_H
