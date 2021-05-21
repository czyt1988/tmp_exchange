#ifndef SACUSTOMPLOT_H
#define SACUSTOMPLOT_H
#include "qcustomplot.h"

/**
 * @brief 统一为matplotlib类似的接口
 */
class SACustomPlot : public QCustomPlot
{
public:
    SACustomPlot(QWidget *p = nullptr);
    //legend显示
    void showLegend(bool isshow = true);
    bool isLegendVisible() const;
    //允许缩放
    void enableWheelZoom(bool c = true);
    bool isEnableWheelZoom() const;
    //恢复缩放Range
    void resetZoomRange();
    //允许拖动
    void enableRangeDrag(bool c = true);
    bool isEnableRangeDrag() const;
};

#endif // SACUSTOMPLOT_H
