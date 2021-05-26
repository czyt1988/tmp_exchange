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
    class _TracerItem {
    public:
        _TracerItem(SACustomPlotYValueTracer *par,QCPGraph* g);
        ~_TracerItem();
        void create();
        SACustomPlotYValueTracer *m_par;
        QCPGraph * m_graph;
        QCPItemTracer* m_tracer;        // 跟踪的点
        QCPItemText* m_label;           //显示内容
//        QCPItemStraightLine* m_lineV;   //垂直线
        QCPItemLine* m_linkLine;        //箭头
        //一次，此函数配合create调用
        void destory();
        //关联的图
        const QCPGraph* graph() const;
        QCPGraph* graph();
        //判断是否有效
        bool isValid() const;

        //显示控制
        void setVisible(bool on);

        //设置图层，默认图层在overlay，所有元素设置成功返回true
        bool setLayer(const QString& layerName);

        //设置tracker的大小
        void setTracerSize(double s);
        int getTracerSize() const;
        //绘制
        void setTracerPos(const QPointF& p);
        void setLabelPos(const QPointF& p);
        void setLabelText(const QString& str);
        //
        void setTracerPen(const QColor& clr);
        void setTracerBrush(const QColor& clr);
        //按照graph更新tracer的颜色
        void updateColor();
    };

    class _TracerItemDrawData{
    public:
        QSharedPointer<_TracerItem> mItem;
        QPointF mPlotCoords;
        //通过y值排序
        bool operator <(const _TracerItemDrawData& other) const;
    };
public:
    SACustomPlotYValueTracer(QCustomPlot *fig);
    ~SACustomPlotYValueTracer();

    //显示控制
    virtual void setVisible(bool on);
    virtual bool isVisible() const;

    //核心函数，更新绘图的关键，传入当前鼠标的位置给tracer，子类继承此函数实现显示
    virtual void updateByPixelPosition(int x, int y);

    //设置图层，默认图层在overlay，所有元素设置成功返回true
    void setLayer(const QString& layerName);

protected:
    void draw(int x, int y,QList<_TracerItemDrawData> datas);

private:
    QMap<QCPGraph *, QSharedPointer<_TracerItem> > m_graphToTracerItem;
    bool m_isVisible;
    int m_lastX;
    int m_lastY;
};

#endif // SACUSTOMPLOTYVALUETRACER_H
