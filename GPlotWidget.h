#ifndef GPLOTWIDGET_H
#define GPLOTWIDGET_H

#include <QWidget>
#include "GHvacDataInfo.h"
#include "GTemplate.h"
#include <QStandardItemModel>
namespace Ui {
class GPlotWidget;
}

class GPlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GPlotWidget(QWidget *parent = nullptr);
    ~GPlotWidget();
    void setData(const GHvacDataInfo& d);
    void setTemplate(GTemplate *temp);

private:
    void resetTreeView();
    QString findNameBySrc(const QList<GNodeInfo>& infos, const QString& src);
    void makeTree(int itemDataType);

private:
    Ui::GPlotWidget *ui;
    GHvacDataInfo m_data;
    QStandardItemModel *m_treeModel;
    GTemplate *mTemplate;
};

#endif // GPLOTWIDGET_H
