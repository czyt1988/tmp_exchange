#ifndef SACUSTOMPLOT_H
#define SACUSTOMPLOT_H
#include "qcustomplot.h"
#include <QScopedPointer>
class QMouseEvent;
class SACustomPlotPrivate;

/**
 * @brief 以QCustomPlot为基础的二次封装，添加一些附加功能，
 * 统一一些接口
 */
class SACustomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    SACustomPlot(QWidget *p = nullptr);
    ~SACustomPlot();
    //legend显示
    void showLegend(bool isshow = true);
    bool isLegendVisible() const;

    //允许缩放
    void enableWheelZoom(bool c = true);
    bool isEnableWheelZoom() const;

    //允许通过选框进行缩放
    void enableSelectRectZoom(bool c = true);
    bool isEnableSelectRectZoom() const;

    //恢复缩放Range
    void resetZoomRange();

    //允许拖动
    void enableRangeDrag(bool c = true);
    bool isEnableRangeDrag() const;

    //允许坐标轴可选
    void enableAxesSelect(bool c = true);
    bool isEnableAxesSelect() const;

    //允许图例可选
    void enableLegendSelect(bool c = true);
    bool isEnableLegendSelect() const;

    //允许绘图元素可选择
    void enablePlottablesSelect(bool c = true);
    bool isEnablePlottablesSelect() const;

    //设置在坐标轴可选状态下时的拖动限制，在选中x轴时只能拖动x轴方向（水平），
    // 选中y轴时只能拖动y轴方向（垂直）
    void setAxisSelectedDragLimit(bool c = true);
    bool isAxisSelectedDragLimit() const;

    //设置在坐标轴可选状态下时的缩放限制，在选中x轴时只能缩放x轴方向（水平），
    //选中y轴时只能缩放y轴方向（垂直）
    void setAxisSelectedWheelZoomLimit(bool c = true);
    bool isAxisSelectedWheelZoomLimit() const;

    //安装Y轴数值捕获器
    bool installYValueTracer();
    bool isInstallYValueTracer() const;
    void uninstallYValueTracer();

signals:

protected:
    virtual void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private slots:
    void onMousePress();
    void onMouseWheel();

private:
    QScopedPointer<SACustomPlotPrivate> d_ptr;
};

#endif // SACUSTOMPLOT_H
