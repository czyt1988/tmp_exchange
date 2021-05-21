#include "SAColorMap.h"

static SAColorList s_cmap_tab10 = SAColorList({
    QColor(31, 119, 180),
    QColor(254, 126, 13),
    QColor(43, 159, 43),
    QColor(213, 38, 39),
    QColor(148, 103, 189),
    QColor(139, 85, 74),
    QColor(226, 118, 193),
    QColor(126, 126, 126),
    QColor(187, 188, 33),
    QColor(22, 189, 206)
});
static SAColorList s_cmap_set1 = SAColorList({
    QColor(228, 26, 28),
    QColor(54, 125, 183),
    QColor(77, 175, 74),
    QColor(151, 77, 162),
    QColor(254, 126, 1),
    QColor(254, 254, 50),
    QColor(165, 85, 39),
    QColor(246, 128, 190),
    QColor(152, 152, 152)
});
static SAColorList s_cmap_dark2 = SAColorList({
    QColor(26, 157, 118),
    QColor(216, 94, 1),
    QColor(216, 94, 1),
    QColor(230, 40, 137),
    QColor(101, 165, 29),
    QColor(229, 170, 1),
    QColor(165, 117, 28),
    QColor(101, 101, 101)
});
SAColorMap::SAColorMap() : m_colorIndex(-1)
{
}

SAColorMap::~SAColorMap()
{

}


const SAColorList& SAColorMap::colorList() const
{
    return (m_colorlist);
}


SAColorList& SAColorMap::colorList()
{
    return (m_colorlist);
}


/**
 * @brief 获取colorlist的下一个颜色
 * @return 如果colorlist为空，返回QColor(),否则一直循环获取下一个颜色
 */
QColor SAColorMap::nextColor()
{
    if (0 == colorList().size()) {
        m_colorIndex = -1;
        return (QColor());
    }
    ++m_colorIndex;
    if ((m_colorIndex >= 0) && (m_colorIndex < colorList().size())) {
        return (colorList()[m_colorIndex]);
    }else {
        //说明溢出，重置为0
        m_colorIndex = 0;
    }
    return (colorList()[m_colorIndex]);
}


/**
 * @brief 重置颜色索引
 */
void SAColorMap::resetIndex()
{
    m_colorIndex = -1;
}


void SAColorMap::setColorMap(int name)
{
    switch (name)
    {
    case Tab10:
        m_colorlist = s_cmap_tab10;
        break;

    case Set1:
        m_colorlist = s_cmap_set1;
        break;

    case Dark2:
        m_colorlist = s_cmap_dark2;
        break;

    default:
        m_colorlist = s_cmap_tab10;
    }
    resetIndex();
}
