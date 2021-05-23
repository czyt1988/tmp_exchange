#include "SACustomPlotYValueTracer.h"
#include <QPair>

SACustomPlotYValueTracer::_TracerItem::_TracerItem(SACustomPlotYValueTracer *par)
    : m_par(par)
{
    QCustomPlot *fig = par->figure();

    m_tracer = new QCPItemTracer(fig);
    m_label = new QCPItemText(fig);
    m_lineV = new QCPItemStraightLine(fig);
    m_linkLine = new QCPItemLine(fig);

    //默认设置
    m_tracer->setStyle(QCPItemTracer::tsCircle);
    m_tracer->setPen(QColor(Qt::red));
    m_tracer->setBrush(QColor(Qt::red));
    //设置文本
    m_label->setClipToAxisRect(false);
    m_label->setPadding(QMargins(3, 3, 3, 3));
    m_label->setBrush(Qt::NoBrush);
    m_label->setPen(QColor(Qt::black));
    m_label->position->setParentAnchor(m_tracer->position);
    //设置垂直线
    m_lineV->setPen(QColor(Qt::black));
    m_lineV->setClipToAxisRect(true);
    m_lineV->point1->setCoords(0, 0);
    m_lineV->point2->setCoords(0, 0);
    //
    m_linkLine->setPen(QColor(Qt::black));
    m_linkLine->setClipToAxisRect(false);
    m_linkLine->setHead(QCPLineEnding::esNone);
    setLayer("overlay");
}


SACustomPlotYValueTracer::_TracerItem::~_TracerItem()
{
    QCustomPlot *fig = m_par->figure();

    if (fig) {
        if (m_tracer) {
            fig->removeItem(m_tracer);
        }
        if (m_label) {
            fig->removeItem(m_label);
        }
        if (m_lineV) {
            fig->removeItem(m_lineV);
        }
        if (m_linkLine) {
            fig->removeItem(m_linkLine);
        }
    }
}


void SACustomPlotYValueTracer::_TracerItem::setVisible(bool on)
{
    if (m_tracer) {
        m_tracer->setVisible(on);
    }
    if (m_label) {
        m_label->setVisible(on);
    }
    if (m_lineV) {
        m_lineV->setVisible(on);
    }
    if (m_linkLine) {
        m_linkLine->setVisible(on);
    }
}


bool SACustomPlotYValueTracer::_TracerItem::setLayer(const QString& layerName)
{
    bool ok = true;

    if (m_tracer) {
        ok &= (m_tracer->setLayer(layerName));
    }
    if (m_label) {
        ok &= (m_label->setLayer(layerName));
    }
    if (m_lineV) {
        ok &= (m_lineV->setLayer(layerName));
    }
    if (m_linkLine) {
        ok &= (m_linkLine->setLayer(layerName));
    }
    return (ok);
}


SACustomPlotYValueTracer::SACustomPlotYValueTracer(QCustomPlot *fig)
    : SACustomPlotAbstractValueTracer(fig)
    , m_isVisible(true)
    , m_lastX(-1)
    , m_lastY(-1)
{
}


SACustomPlotYValueTracer::~SACustomPlotYValueTracer()
{
}


void SACustomPlotYValueTracer::setVisible(bool on)
{
    for (auto& it : m_graphToTracerItem)
    {
        it.setVisible(on);
    }
    m_isVisible = on;
    //从隐藏到显示，先更新一下最后状态的
    if (on) {
        updateByPixelPosition(m_lastX, m_lastY);
    }
}


bool SACustomPlotYValueTracer::isVisible() const
{
    return (m_isVisible);
}


void SACustomPlotYValueTracer::updateByPixelPosition(int x, int y)
{
    m_lastX = x;
    m_lastY = y;
    if (!isVisible()) {
        //不可见，仅保存最后的点，不做任何动作
        return;
    }
    QCustomPlot *fig = figure();

    if (nullptr == fig) {
        return;
    }
    QCPAxisRect *axis = fig->axisRectAt(QPointF(x, y));

    if (nullptr == axis) {
        return;
    }
    QList<QCPGraph *> gs = axis->graphs();
    int gssize = gs.size();

    if (0 == gssize) {
        return;
    }
    //迭代
    //找出所有要标记的点
    QList<QPair<QCPGraph *, QPointF> > gsandvalues;//一次性获取所有的实际点

    for (int i = 0; i < gssize; ++i)
    {
        QCPGraph *g = gs[i];
        double x_val = g->keyAxis()->pixelToCoord(x);
        auto iter = g->data()->findBegin(x_val);
        if (iter == g->data()->end()) {
            continue;
        }
        double y_val = iter->mainValue();
        gsandvalues.append(qMakePair(g, QPointF(x_val, y_val)));
    }
    //绘制
    draw(gsandvalues);
}


/**
 * @brief 设置图层，默认图层在overlay，所有元素设置成功返回true
 * @param layerName
 * @return
 */
bool SACustomPlotYValueTracer::setLayer(const QString& layerName)
{
    for (auto& it : m_graphToTracerItem)
    {
        it.setLayer(layerName);
    }
}


void SACustomPlotYValueTracer::draw(QList<QPair<QCPGraph *, QPointF> >& datas)
{
    for (QPair<QCPGraph *, QPointF>& info : datas)
    {
        QCPGraph *graph = info.first;
        if (info.second.y() > graph->valueAxis()->range().upper) {
            info.second.ry() = graph->valueAxis()->range().upper;
        }
//        m_tracer->position->setCoords(xValue, 1);
    }
}
