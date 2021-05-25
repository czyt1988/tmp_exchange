#include "GTemplate.h"
#include <QFile>
#include <QDebug>
GTemplate::GTemplate(QObject *par) : QObject(par)
{
    mSystemModel = new QStandardItemModel(this);
    mSystemModel->setHorizontalHeaderLabels({ QStringLiteral("参数"), QStringLiteral("值"), QStringLiteral("单位") });
    mIduModel = new GIDUTableModel(this);
    clear();
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


const QList<GNodeInfo>& GTemplate::getSystemInfoList() const
{
    return (mSystemInfo);
}


const QList<GNodeInfo>& GTemplate::getIduInfoList() const
{
    return (mIduInfo);
}


const QList<GNodeInfo>& GTemplate::getModuleFaultInfoList() const
{
    return (mModuleFaultSrc);
}


const QList<GNodeInfo>& GTemplate::getSystemFaultInfoList() const
{
    return (mSystemFaultSrc);
}


const QList<GNodeInfo>& GTemplate::getIduFaultInfoList() const
{
    return (mIduFaultSrc);
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
    loadsetting(rootEle);
}


void GTemplate::loadsetting(QDomElement& root)
{
    QDomElement settag = root.firstChildElement("setting");

    if (settag.isNull()) {
        return;
    }
    QDomNodeList cl = settag.childNodes();

    for (int i = 0; i < cl.size(); ++i)
    {
        if (!cl.at(i).isElement()) {
            continue;
        }
        QDomElement e = cl.at(i).toElement();
        if (e.tagName() == "system-fault") {
            mSystemFaultSrc.clear();
            loadFaultInfo(e, mSystemFaultSrc);
        }else if (e.tagName() == "module-fault") {
            mModuleFaultSrc.clear();
            loadFaultInfo(e, mModuleFaultSrc);
        }else if (e.tagName() == "idu-fault") {
            mIduFaultSrc.clear();
            loadFaultInfo(e, mIduFaultSrc);
        }
    }
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
//            qDebug() << gname;
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

//    qDebug() << ni;
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


void GTemplate::loadFaultInfo(QDomElement& faultEle, QList<GNodeInfo>& res)
{
    QDomNodeList cl = faultEle.childNodes();

    for (int i = 0; i < cl.size(); ++i)
    {
        if (!cl.at(i).isElement()) {
            continue;
        }
        QDomElement e = cl.at(i).toElement();
        if (e.tagName() == "node") {
            GNodeInfo ni;
            ni.mName = e.attribute("name");
            ni.mSrc = e.attribute("ds");
            //故障节点其余信息不需要
            res.append(ni);
        }
    }
}
