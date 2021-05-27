#include "SACustomPlotYValueTracer.h"
#include <QPair>
#include <QDebug>

SACustomPlotYValueTracer::_TracerItem::_TracerItem(SACustomPlotYValueTracer *par, QCPGraph* g)
    : m_par(par)
    ,m_graph(g)
    ,m_tracer(nullptr)
    ,m_label(nullptr)
//    ,m_lineV(nullptr)
    ,m_linkLine(nullptr)
{
    create();
}


SACustomPlotYValueTracer::_TracerItem::~_TracerItem()
{
    destory();
}

void SACustomPlotYValueTracer::_TracerItem::create()
{
    QCustomPlot *fig = m_par->figure();
    m_tracer = new QCPItemTracer(fig);
    m_label = new QCPItemText(fig);
//    m_lineV = new QCPItemStraightLine(fig);
    m_linkLine = new QCPItemLine(fig);

    //默认设置
    m_tracer->setStyle(QCPItemTracer::tsCircle);
    m_tracer->setPen(QColor(Qt::red));
    m_tracer->setBrush(QColor(Qt::red));
    m_tracer->setSize(5);
    m_tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
    m_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
    //设置文本
    m_label->setClipToAxisRect(false);
    m_label->setPadding(QMargins(15, 5, 5, 5));
    m_label->setBrush(QColor(21,21,21,100));
    m_label->setColor(QColor(Qt::white));
    m_label->setPositionAlignment(Qt::AlignVCenter|Qt::AlignLeft);
//    m_label->position->setTypeX(QCPItemPosition::ptPlotCoords);
//    m_label->position->setTypeY(QCPItemPosition::ptPlotCoords);
    //这时label的坐标相对于m_tracer
    m_label->position->setParentAnchor(m_tracer->position);
    m_label->position->setCoords(30,-20);
    //设置垂直线
//    m_lineV->setPen(QColor(Qt::black));
//    m_lineV->setClipToAxisRect(true);
//    m_lineV->point1->setCoords(0, 0);
//    m_lineV->point2->setCoords(0, 0);
    //
    m_linkLine->setPen(QColor(Qt::black));
    m_linkLine->setClipToAxisRect(false);
    m_linkLine->setHead(QCPLineEnding::esSpikeArrow);
    m_linkLine->setTail(QCPLineEnding::esDiamond);
    m_linkLine->end->setParentAnchor(m_tracer->position);
    m_linkLine->end->setCoords(0, 0);
    m_linkLine->start->setParentAnchor(m_label->position);
    m_linkLine->start->setCoords(0, 0);//偏移量
    setLayer("overlay");
}

void SACustomPlotYValueTracer::_TracerItem::destory()
{
    QCustomPlot *fig = m_par->figure();

    if (fig) {
        if (m_tracer) {
            fig->removeItem(m_tracer);
        }
        if (m_label) {
            fig->removeItem(m_label);
        }
//        if (m_lineV) {
//            fig->removeItem(m_lineV);
//        }
        if (m_linkLine) {
            fig->removeItem(m_linkLine);
        }
    }
}

const QCPGraph *SACustomPlotYValueTracer::_TracerItem::graph() const
{
    return m_graph;
}

QCPGraph *SACustomPlotYValueTracer::_TracerItem::graph()
{
    return m_graph;
}

bool SACustomPlotYValueTracer::_TracerItem::isValid() const
{
    return (m_tracer != nullptr);
}


void SACustomPlotYValueTracer::_TracerItem::setVisible(bool on)
{
    if (m_tracer) {
        m_tracer->setVisible(on);
    }
    if (m_label) {
        m_label->setVisible(on);
    }
//    if (m_lineV) {
//        m_lineV->setVisible(on);
//    }
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
//    if (m_lineV) {
//        ok &= (m_lineV->setLayer(layerName));
//    }
    if (m_linkLine) {
        ok &= (m_linkLine->setLayer(layerName));
    }
    return (ok);
}

void SACustomPlotYValueTracer::_TracerItem::setTracerSize(double s)
{
    if (m_tracer) {
        m_tracer->setSize(s);
    }
}

int SACustomPlotYValueTracer::_TracerItem::getTracerSize() const
{
    if (m_tracer) {
        return m_tracer->size();
    }
    return -1;
}

void SACustomPlotYValueTracer::_TracerItem::setTracerPos(const QPointF &p)
{
    m_tracer->position->setCoords(p.x(), p.y());
}

void SACustomPlotYValueTracer::_TracerItem::setLabelPos(const QPointF &p)
{
    m_label->position->setCoords(p.x(), p.y());
}

void SACustomPlotYValueTracer::_TracerItem::setLabelText(const QString &str)
{
    m_label->setText(str);
}

void SACustomPlotYValueTracer::_TracerItem::setTracerPen(const QColor &clr)
{
    if(m_tracer->pen().color() == clr){
        return;
    }
    m_tracer->setPen(clr);
}

void SACustomPlotYValueTracer::_TracerItem::setTracerBrush(const QColor &clr)
{
    if(m_tracer->brush().color() == clr){
        return;
    }
    m_tracer->setBrush(clr);
}

void SACustomPlotYValueTracer::_TracerItem::updateColor()
{
    QCPGraph* g = graph();
    if(nullptr == g){
        return;
    }
    QColor c = g->pen().color();
    setTracerPen(c);
    setTracerBrush(c);
    m_linkLine->setPen(QPen(c,1,Qt::DashLine));
}


//////////////////////////////////////////////////////////

//
bool SACustomPlotYValueTracer::_TracerItemDrawData::operator <(const SACustomPlotYValueTracer::_TracerItemDrawData &other) const
{
    //从大到小排序
    return (this->mPlotCoords.y() > other.mPlotCoords.y());
}
/////////////////////////////////////////////////////////


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
        it->setVisible(on);
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
    QList<_TracerItemDrawData> drawData;//一次性获取所有的实际点
    for (int i = 0; i < gssize; ++i)
    {
        //查找item
        QCPGraph *g = gs[i];
        QSharedPointer<_TracerItem> item(nullptr);
        auto ite = m_graphToTracerItem.find(g);
        if(ite == m_graphToTracerItem.end()){
            if(g->visible()){
                //如果graph是可见的，且map里没有，就创建一个
                item = QSharedPointer<_TracerItem>::create(this,g);
                item->setVisible(true);
                m_graphToTracerItem[g] = item;
            }else{
                //这个一定要有
                continue;
            }
        }else{
            item = m_graphToTracerItem[g];
        }
        if(!g->visible()){
            //不显示的就忽略
            item->setVisible(false);
            continue;
        }


        item->updateColor();
        double x_val = g->keyAxis()->pixelToCoord(x);
        auto iterpoint = g->data()->findBegin(x_val);
        if (iterpoint == g->data()->end()) {
            continue;
        }
        _TracerItemDrawData d;
        d.mItem = item;
        d.mPlotCoords = QPointF(iterpoint->mainKey(), iterpoint->mainValue());
        drawData.append(d);
    }
    //删除不在QCPGraph
    //绘制
    draw(x,y,drawData);
}


/**
 * @brief 设置图层，默认图层在overlay，所有元素设置成功返回true
 * @param layerName
 * @return
 */
void SACustomPlotYValueTracer::setLayer(const QString& layerName)
{
    for (auto& it : m_graphToTracerItem)
    {
        it->setLayer(layerName);
    }
}


void SACustomPlotYValueTracer::draw(int x, int y, QList<_TracerItemDrawData> datas)
{
    //从小到大排序
    std::sort(datas.begin(),datas.end());
    //先绘制捕获的点
    const int size = datas.size();
    for (int i=0;i<size;++i)
    {
        QSharedPointer<_TracerItem> item = datas[i].mItem;
        QPointF pos = datas[i].mPlotCoords;
        if(nullptr == item || !(item->isValid())){
            continue;
        }
        item->setTracerPos(pos);
    }
    //m_label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    //逐个生成文本框
    for (int i=0;i<size;++i)
    {
        QSharedPointer<_TracerItem> item = datas[i].mItem;
        QPointF pos = datas[i].mPlotCoords;
        item->setLabelText(QStringLiteral("%1:%2").arg(item->graph()->name()).arg(pos.y()));
    }
}


