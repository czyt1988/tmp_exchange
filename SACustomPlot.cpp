#include "SACustomPlot.h"
#include <QMouseEvent>
#include "SACustomPlotYValueTracer.h"

class SACustomPlotPrivate {
    friend class SACustomPlot;
public:
    SACustomPlotPrivate(SACustomPlot *p);
public:
    SACustomPlot *q_ptr;
    bool mIsAxisSelectedDragLimit;
    bool mIsAxisSelectedWheelZoomLimit;
    QSharedPointer<SACustomPlotYValueTracer> mYValTracer;
};

SACustomPlotPrivate::SACustomPlotPrivate(SACustomPlot *p) : q_ptr(p)
    , mIsAxisSelectedDragLimit(true)
    , mIsAxisSelectedWheelZoomLimit(true)
{
}


SACustomPlot::SACustomPlot(QWidget *p) : QCustomPlot(p)
    , d_ptr(new SACustomPlotPrivate(this))
{
    connect(this, &QCustomPlot::mousePress, this, &SACustomPlot::onMousePress);
    connect(this, &QCustomPlot::mouseWheel, this, &SACustomPlot::onMouseWheel);
}


SACustomPlot::~SACustomPlot()
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
    return (this->legend->visible());
}


/**
 * @brief 允许缩放
 * @param c
 */
void SACustomPlot::enableWheelZoom(bool c)
{
    setInteraction(QCP::iRangeZoom, c);
}


/**
 * @brief 判断是否允许缩放
 * @return
 */
bool SACustomPlot::isEnableWheelZoom() const
{
    return (interactions().testFlag(QCP::iRangeZoom));
}


/**
 * @brief 允许通过选框进行缩放
 * @note 选框缩放和拖动不能共存，选框优先
 * @param c
 */
void SACustomPlot::enableSelectRectZoom(bool c)
{
    if (c) {
        setSelectionRectMode(QCP::SelectionRectMode::srmZoom);  //模式：框选放大
    }else{
        setSelectionRectMode(QCP::SelectionRectMode::srmNone);  //否则无任何模式
    }
}


/**
 * @brief 判断是否允许通过选框进行缩放
 * @return
 */
bool SACustomPlot::isEnableSelectRectZoom() const
{
    return (selectionRectMode() == (QCP::SelectionRectMode::srmZoom));
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
    setInteraction(QCP::iRangeDrag, c);
}


/**
 * @brief 判断是否允许拖动
 * @return
 */
bool SACustomPlot::isEnableRangeDrag() const
{
    return (interactions().testFlag(QCP::iRangeDrag));
}


/**
 * @brief 允许坐标轴可选
 *
 * 通过@sa setAxisSelectedDragLimit 可以对坐标轴选中做一些特殊设置
 * @param c
 */
void SACustomPlot::enableAxesSelect(bool c)
{
    setInteraction(QCP::iSelectAxes, c);
}


/**
 * @brief 判断是否允许坐标轴可选
 * @return
 */
bool SACustomPlot::isEnableAxesSelect() const
{
    return (interactions().testFlag(QCP::iSelectAxes));
}


/**
 * @brief 图例可选
 * @param c
 */
void SACustomPlot::enableLegendSelect(bool c)
{
    setInteraction(QCP::iSelectLegend, c);
}


/**
 * @brief 判断是否图例可选
 * @return
 */
bool SACustomPlot::isEnableLegendSelect() const
{
    return (interactions().testFlag(QCP::iSelectLegend));
}


/**
 * @brief 绘图元素可选择
 * @param c
 */
void SACustomPlot::enablePlottablesSelect(bool c)
{
    setInteraction(QCP::iSelectPlottables, c);
}


/**
 * @brief 判断是否绘图元素可选择
 * @return
 */
bool SACustomPlot::isEnablePlottablesSelect() const
{
    return (interactions().testFlag(QCP::iSelectPlottables));
}


/**
 * @brief 设置在坐标轴可选状态下时的拖动限制，
 * 在选中x轴时只能拖动x轴方向（水平），
 * 选中y轴时只能拖动y轴方向（垂直）
 * @param c
 */
void SACustomPlot::setAxisSelectedDragLimit(bool c)
{
    d_ptr->mIsAxisSelectedDragLimit = c;
}


/**
 * @brief 判断是否设置在坐标轴可选状态下时的拖动限制
 * @sa setAxisSelectedDragLimit
 * @return
 */
bool SACustomPlot::isAxisSelectedDragLimit() const
{
    return (d_ptr->mIsAxisSelectedDragLimit);
}


/**
 * @brief 设置在坐标轴可选状态下时的缩放限制，
 * 在选中x轴时只能缩放x轴方向（水平），
 * 选中y轴时只能缩放y轴方向（垂直）
 * @param c
 */
void SACustomPlot::setAxisSelectedWheelZoomLimit(bool c)
{
    d_ptr->mIsAxisSelectedWheelZoomLimit = c;
}


/**
 * @brief 判断是否设置在坐标轴可选状态下时的缩放限制
 * @return
 */
bool SACustomPlot::isAxisSelectedWheelZoomLimit() const
{
    return (d_ptr->mIsAxisSelectedWheelZoomLimit);
}

/**
 * @brief 安装y轴数值捕获器
 * @return
 */
bool SACustomPlot::installYValueTracer()
{
    d_ptr->mYValTracer = QSharedPointer<SACustomPlotYValueTracer>::create(this);
}
/**
 * @brief 判断y轴数值捕获器是否安装
 * @return
 */
bool SACustomPlot::isInstallYValueTracer() const
{
    return (d_ptr->mYValTracer != nullptr);
}
/**
 * @brief 卸载y轴数值捕获器
 */
void SACustomPlot::uninstallYValueTracer()
{
    d_ptr->mYValTracer.reset();
}


/**
 * @brief 在设置了点捕获的情况下，需要通过鼠标移动事件来监听捕获的点
 * @param e
 */
void SACustomPlot::mouseMoveEvent(QMouseEvent *e)
{
    QCustomPlot::mouseMoveEvent(e);
    if(isInstallYValueTracer()){
        d_ptr->mYValTracer->updateByPixelPosition(e->x(),e->y());
        replot(QCustomPlot::rpQueuedReplot);
    }
}


void SACustomPlot::onMousePress()
{
    if (d_ptr->mIsAxisSelectedDragLimit) {
        if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            this->axisRect()->setRangeDrag(this->xAxis->orientation());
        }else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            this->axisRect()->setRangeDrag(this->yAxis->orientation());
        }else{
            this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
        }
    }
}


void SACustomPlot::onMouseWheel()
{
    if (d_ptr->mIsAxisSelectedWheelZoomLimit) {
        if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            this->axisRect()->setRangeZoom(this->xAxis->orientation());
        }else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            this->axisRect()->setRangeZoom(this->yAxis->orientation());
        }else{
            this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
        }
    }
}
