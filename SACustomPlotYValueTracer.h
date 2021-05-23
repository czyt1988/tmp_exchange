#ifndef SACUSTOMPLOTYVALUETRACER_H
#define SACUSTOMPLOTYVALUETRACER_H
#include "SACustomPlotAbstractValueTracer.h"

/**
 * @brief 用于捕获显示y轴值的tracer
 *
 * 默认绘制在overlay图层上，可以通过setLayer函数改变图层
 */
class SACustomPlotYValueTracer : public SACustomPlotAbstractValueTracer
{
public:
    SACustomPlotYValueTracer(QCustomPlot *fig);
    ~SACustomPlotYValueTracer();

    //显示控制
    virtual void setVisible(bool on);
    virtual bool isVisible() const;

    //核心函数，更新绘图的关键，传入当前鼠标的位置给tracer，子类继承此函数实现显示
    virtual void updateByPixelPosition(int x, int y);

    //设置图层，默认图层在overlay，所有元素设置成功返回true
    bool setLayer(const QString& layerName);

protected:
    void draw(QList<QPair<QCPGraph *, QPointF> >& datas);

private:
    class _TracerItem {
public:
        _TracerItem(SACustomPlotYValueTracer *par);
        ~_TracerItem();
        QCPItemTracer *m_tracer;        // 跟踪的点
        QCPItemText *m_label;           //显示内容
        QCPItemStraightLine *m_lineV;   //垂直线
        QCPItemLine *m_linkLine;        //箭头
        SACustomPlotYValueTracer *m_par;
        //显示控制
        void setVisible(bool on);

        //设置图层，默认图层在overlay，所有元素设置成功返回true
        bool setLayer(const QString& layerName);
    };
    QMap<QCPGraph *, _TracerItem> m_graphToTracerItem;
    bool m_isVisible;
    int m_lastX;
    int m_lastY;
};

#endif // SACUSTOMPLOTYVALUETRACER_H
