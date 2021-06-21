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

/**
 * @brief 绘图窗口
 */
class GPlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GPlotWidget(QWidget *parent = nullptr);
    ~GPlotWidget();
    void setData(const GHvacDataInfo& d);
    void setTemplate(GTemplate *temp);
    void resetSplitterRatio();
    SACustomPlot *figure();

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
    void search(const QString& keyword);
    void clearAllState(QStandardItemModel *model);
    void clearStandardItemState(QStandardItem *item);
    QStandardItem *findByID(qulonglong id);
    QStandardItem *findByID(QStandardItem *par, qulonglong id);

private slots:
    void onModelItemChanged(QStandardItem *item);
    void onSearchModelItemChanged(QStandardItem *item);
    void onTreeViewDoubleClicked(const QModelIndex& index);
    void on_pushButtonSearch_clicked();

    void on_lineEditSearch_textChanged(const QString& arg1);

private:
    Ui::GPlotWidget *ui;
    GHvacDataInfo m_data;
    QStandardItemModel *m_treeModel;
    QStandardItemModel *m_searchModel;
    GTemplate *m_template;
    QMap<GHvacDataInfo::SeriesPtr, QCPGraph *> m_ptrToGraph;
    SAColorMap m_colormap;
};

#endif // GPLOTWIDGET_H
