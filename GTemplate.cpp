#include "GTemplate.h"
#include <QFile>
#include <QDebug>
GTemplate::GTemplate(QObject *par) : QObject(par)
{
    mSystemModel = new QStandardItemModel(this);
    mSystemModel->setHorizontalHeaderLabels({ QStringLiteral("参数"), QStringLiteral("值"), QStringLiteral("单位") });
    mIduModel = new GIDUTableModel(this);
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
    QDomDocument doc;

    if (!doc.setContent(&f, &err)) {
        qDebug() << QStringLiteral("文件异常:") << err;
        return (false);
    }
    parse(doc);
    return (true);
}


/**
 * @brief 清空
 */
void GTemplate::clear()
{
    mName = "";
    mSystemModel->clear();
    mIduModel->clear();
    mValueItemToNode.clear();
    mNodeToItemValue.clear();
    mModuleInfo.clear();
    mSystemInfo.clear();
    mIduInfo.clear();
    mSystemModel->setHorizontalHeaderLabels({ QStringLiteral("参数"), QStringLiteral("值"), QStringLiteral("单位") });
}


QString GTemplate::name() const
{
    return (mName);
}


QStandardItemModel *GTemplate::getSystemModel() const
{
    return (mSystemModel);
}


GIDUTableModel *GTemplate::getIduModel() const
{
    return (mIduModel);
}


const QList<GNodeInfo>& GTemplate::getModuleInfoList() const
{
    return (mModuleInfo);
}


void GTemplate::updateValue(const GNodeInfo& value)
{
    auto i = mNodeToItemValue.find(value);

    if (i == mNodeToItemValue.end()) {
        return;
    }
    i.value()->setText(value.mDisplayValue);
}


void GTemplate::parse(QDomDocument& doc)
{
    QDomElement rootEle = doc.documentElement();

    mName = rootEle.attribute("name");
    loadSystemInfo(rootEle);
    loadModuleInfo(rootEle);
    loadIduInfo(rootEle);
}


void GTemplate::loadSystemInfo(QDomElement& root)
{
    QDomElement systag = root.firstChildElement("system");

    if (systag.isNull()) {
        return;
    }
    QDomNodeList cl = systag.childNodes();


    for (int i = 0; i < cl.size(); ++i)
    {
        if (!cl.at(i).isElement()) {
            continue;
        }
        QDomElement e = cl.at(i).toElement();
        if (e.tagName() == "group") {
            QString gname = e.attribute("name");
            qDebug() << gname;
            QStandardItem *gi = new QStandardItem(gname);
            QDomNodeList nodes = e.childNodes();
            for (int j = 0; j < nodes.size(); ++j)
            {
                if (!nodes.at(j).isElement()) {
                    continue;
                }
                QDomElement ge = nodes.at(j).toElement();
                if (ge.tagName() == "node") {
                    loadSystemItemFromNode(ge, gi);
                }
            }
            mSystemModel->appendRow(gi);
        }else if (e.tagName() == "node") {
            loadSystemItemFromNode(e, nullptr);
        }
    }
}


bool GTemplate::loadSystemItemFromNode(QDomElement& nodeitem, QStandardItem *parItem)
{
    GNodeInfo ni;

    ni.mName = nodeitem.attribute("name");
    ni.mSrc = nodeitem.attribute("ds");
    ni.mSuffix = nodeitem.attribute("suffix");
    if (ni.mName.isEmpty() || ni.mSrc.isEmpty()) {
        qDebug() << QStringLiteral("模板文件中,system下的node节点存在name和ds都为空的情况");
        return (false);
    }
    QStandardItem *gi = new QStandardItem(ni.mName);
    QStandardItem *vi = new QStandardItem(ni.mDisplayValue);
    QStandardItem *ui = new QStandardItem(ni.mSuffix);

    qDebug() << ni;
    if (parItem) {
        int r = parItem->rowCount();
        parItem->setChild(r, 0, gi);
        parItem->setChild(r, 1, vi);
        parItem->setChild(r, 3, ui);
    }else{
        int r = mSystemModel->rowCount();
        mSystemModel->setItem(r, 0, gi);
        mSystemModel->setItem(r, 1, vi);
        mSystemModel->setItem(r, 2, ui);
    }
    mSystemInfo.append(ni);
    mValueItemToNode[vi] = ni;
    mNodeToItemValue[ni] = vi;
    return (true);
}


void GTemplate::loadIduInfo(QDomElement& root)
{
    QDomElement idutag = root.firstChildElement("idu");
    QDomNodeList cl = idutag.childNodes();

    for (int i = 0; i < cl.size(); ++i)
    {
        if (!cl.at(i).isElement()) {
            continue;
        }
        QDomElement e = cl.at(i).toElement();
        if (e.tagName() == "node") {
            loadIduItemFromNode(e);
        }
    }
    mIduModel->setNodeInfo(mIduInfo);
}


bool GTemplate::loadIduItemFromNode(QDomElement& nodeitem)
{
    GNodeInfo ni;

    ni.mName = nodeitem.attribute("name");
    ni.mSrc = nodeitem.attribute("ds");
    ni.mSuffix = nodeitem.attribute("suffix");
    if (ni.mName.isEmpty() || ni.mSrc.isEmpty()) {
        qDebug() << QStringLiteral("模板文件中,idu下的node节点存在name和ds都为空的情况");
        return (false);
    }
    mIduInfo.append(ni);
    return (true);
}


void GTemplate::loadModuleInfo(QDomElement& root)
{
    QDomElement mtag = root.firstChildElement("module");
    QDomNodeList cl = mtag.childNodes();

    for (int i = 0; i < cl.size(); ++i)
    {
        if (!cl.at(i).isElement()) {
            continue;
        }
        QDomElement e = cl.at(i).toElement();
        if (e.tagName() == "node") {
            loadModuleItemFromNode(e);
        }
    }
}


bool GTemplate::loadModuleItemFromNode(QDomElement& nodeitem)
{
    GNodeInfo ni;

    ni.mName = nodeitem.attribute("name");
    ni.mSrc = nodeitem.attribute("ds");
    ni.mSuffix = nodeitem.attribute("suffix");
    if (ni.mName.isEmpty() || ni.mSrc.isEmpty()) {
        qDebug() << QStringLiteral("模板文件中,Module下的node节点存在name和ds都为空的情况");
        return (false);
    }
    mModuleInfo.append(ni);
    return (true);
}
