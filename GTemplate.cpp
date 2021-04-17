#include "GTemplate.h"
#include <QFile>
#include <QDebug>
GTemplate::GTemplate(QObject *par) : QObject(par)
{
    mSystemModel = new QStandardItemModel(this);
    mIduModel = new QStandardItemModel(this);
}


/**
 * @brief 加载模板
 * @param path 模板路径
 * @return 如果加载错误，也会清空原有数据
 */
bool GTemplate::load(const QString& path)
{
    clear();
    QFile f(path);

    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << QStringLiteral("无法打开文件:") << path;
        return (false);
    }
    QString err;

    if (!mDoc.setContent(&f, &err)) {
        qDebug() << QStringLiteral("文件异常:") << err;
        return (false);
    }
    parse();
}


/**
 * @brief 清空
 */
void GTemplate::clear()
{
    mName = "";
    mSystemModel->clear();
    mIduModel->clear();
    mItemToNode.clear();
    mNodeToItem.clear();
    mModuleInfo.clear();
    mDoc.clear();
}


QStandardItemModel *GTemplate::getSystemModel() const
{
}


QStandardItemModel *GTemplate::getIduModel() const
{
}


const QList<GNodeInfo>& GTemplate::getModuleInfoList() const
{
    return (mModuleInfo);
}


void GTemplate::parse()
{
    QDomElement docElem = mDoc.documentElement();
}
