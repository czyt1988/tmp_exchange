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
};

#endif // SACUSTOMPLOT_H
