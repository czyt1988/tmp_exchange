#include "SACustomPlotAbstractValueTracer.h"

SACustomPlotAbstractValueTracer::SACustomPlotAbstractValueTracer(QCustomPlot *fig)
    : m_figure(fig)
{
}


SACustomPlotAbstractValueTracer::~SACustomPlotAbstractValueTracer()
{
}


QCustomPlot *SACustomPlotAbstractValueTracer::figure()
{
    return (m_figure.data());
}


const QCustomPlot *SACustomPlotAbstractValueTracer::figure() const
{
    return (m_figure.data());
}
