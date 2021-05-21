#include "SACustomPlot.h"

SACustomPlot::SACustomPlot(QWidget *p) : QCustomPlot(p)
{
}

/**
 * @brief 显示legend
 * @param isshow 默认为true
 * @note 会触发刷新
 */
void SACustomPlot::showLegend(bool isshow)
{
    this->legend->setVisible(isshow);
    replot(QCustomPlot::rpQueuedReplot);
}

bool SACustomPlot::isLegendVisible() const
{
    return this->legend->visible();
}

/**
 * @brief 允许缩放
 * @param c
 */
void SACustomPlot::enableWheelZoom(bool c)
{
    setInteraction(QCP::iRangeZoom,c);
}

/**
 * @brief 判断是否允许缩放
 * @return
 */
bool SACustomPlot::isEnableWheelZoom() const
{
    return interactions().testFlag(QCP::iRangeZoom);
}
/**
 * @brief SACustomPlot::resetZoomRange
 */
void SACustomPlot::resetZoomRange()
{
    this->yAxis->scaleRange(1, this->yAxis->range().center());
    this->xAxis->scaleRange(1, this->xAxis->range().center());
}
/**
 * @brief 允许拖动
 * @param c
 */
void SACustomPlot::enableRangeDrag(bool c)
{
    setInteraction(QCP::iRangeDrag,c);
}
/**
 * @brief 判断是否允许拖动
 * @return
 */
bool SACustomPlot::isEnableRangeDrag() const
{
    return interactions().testFlag(QCP::iRangeDrag);
}
