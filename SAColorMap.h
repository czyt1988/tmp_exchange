#ifndef SACOLORMAP_H
#define SACOLORMAP_H
#include "SAColorList.h"

class SAColorMap
{
public:

    /**
     * @brief 定义默认的colormap
     */
    enum ColorMapName {
        Tab10		= 0,    ///< python matplotlib默认颜色
        Set1,                   ///< R ggplot配色之一
        Dark2,
        UserDefine	= 10000
    };
    SAColorMap();
    ~SAColorMap();
    const SAColorList& colorList() const;
    SAColorList& colorList();

    //获取colorlist的下一个颜色
    QColor nextColor();

    //重置颜色索引
    void resetIndex();

    //设置colormap
    void setColorMap(int name);

private:
    SAColorList m_colorlist;
    mutable int m_colorIndex;
};

#endif // SACOLORMAP_H
