#ifndef GPLOTWIDGET_H
#define GPLOTWIDGET_H

#include <QWidget>
#include "GHvacDataInfo.h"
#include "GTemplate.h"
#include <QStandardItemModel>
#include <QMap>
#include "SACustomPlot.h"
#include "SAColorMap.h"
class SACustomPlot;
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
    void resetSplitterRatio();
    SACustomPlot* figure();
signals:

    /**
     * @brief message
     * @param msg
     */
    void message(const QString& msg);
private:
    void resetTreeView();
    QString findNameBySrc(const QList<GNodeInfo>& infos, const QString& src);
    void makeTree(int itemDataType);
    QVector<QCPGraphData> toPlotData(GHvacDataInfo::SeriesPtr x, GHvacDataInfo::SeriesPtr y);

private slots:
    void onModelItemChanged(QStandardItem *item);
    void onTreeViewDoubleClicked(const QModelIndex &index);
private:
    Ui::GPlotWidget *ui;
    GHvacDataInfo m_data;
    QStandardItemModel *m_treeModel;
    GTemplate *mTemplate;
    QMap<GHvacDataInfo::SeriesPtr, QCPGraph *> m_ptrToGraph;
    SAColorMap m_colormap;
};

#endif // GPLOTWIDGET_H
