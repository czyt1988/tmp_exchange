#ifndef SACUSTOMPLOTABSTRACTVALUETRACER_H
#define SACUSTOMPLOTABSTRACTVALUETRACER_H
#include "qcustomplot.h"
#include <QPointer>

/**
 * @brief 所有SACustomPlot点捕获的基类
 */
class SACustomPlotAbstractValueTracer
{
public:
    SACustomPlotAbstractValueTracer(QCustomPlot *fig);
    virtual ~SACustomPlotAbstractValueTracer();
    //获取figure
    QCustomPlot *figure();
    const QCustomPlot *figure() const;

    //显示控制
    virtual void setVisible(bool on) = 0;
    virtual bool isVisible() const = 0;

    //核心函数，更新绘图的关键，传入当前鼠标的位置给tracer，子类继承此函数实现显示
    virtual void updateByPixelPosition(int x, int y) = 0;

private:
    QPointer<QCustomPlot> m_figure;
};

#endif // SACUSTOMPLOTABSTRACTVALUETRACER_H
