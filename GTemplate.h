#ifndef GTEMPLATE_H
#define GTEMPLATE_H

#include <QtCore/qglobal.h>
#include "GNodeInfo.h"
#include <QDomDocument>
#include <QDomElement>
#include <QStandardItemModel>
#include <QHash>
#include "GIDUTableModel.h"

/**
 * @brief 模板描述，界面根据模板生成
 */
class GTemplate : public QObject
{
    Q_OBJECT
public:
    GTemplate(QObject *par = nullptr);
    //加载模板
    bool load(const QString& path);

    //
    void clear();

    //模板名
    QString name() const;

    //
    QStandardItemModel *getSystemModel() const;

    //
    GIDUTableModel *getIduModel() const;

    //获取系统、模块、内机的节点信息
    const QList<GNodeInfo>& getModuleInfoList() const;
    const QList<GNodeInfo>& getSystemInfoList() const;
    const QList<GNodeInfo>& getIduInfoList() const;

    //获取系统、模块、内机的故障信息
    const QList<GNodeInfo>& getModuleFaultInfoList() const;
    const QList<GNodeInfo>& getSystemFaultInfoList() const;
    const QList<GNodeInfo>& getIduFaultInfoList() const;

    //更新数据
    void updateValue(const GNodeInfo& value);


private:
    void parse(QDomDocument& doc);
    void loadsetting(QDomElement& root);
    void loadSystemInfo(QDomElement& root);
    bool loadSystemItemFromNode(QDomElement& nodeitem, QStandardItem *parItem);
    void loadIduInfo(QDomElement& root);
    bool loadIduItemFromNode(QDomElement& nodeitem);
    void loadModuleInfo(QDomElement& root);
    bool loadModuleItemFromNode(QDomElement& nodeitem);

    void loadFaultInfo(QDomElement& faultEle, QList<GNodeInfo>& res);

private:
    QString mName;///< 模板名

    QList<GNodeInfo> mSystemInfo;
    QList<GNodeInfo> mModuleInfo;
    QList<GNodeInfo> mIduInfo;
    QStandardItemModel *mSystemModel;
    GIDUTableModel *mIduModel;
    QHash<QStandardItem *, GNodeInfo> mValueItemToNode;
    QHash<GNodeInfo, QStandardItem *> mNodeToItemValue;

    QList<GNodeInfo> mSystemFaultSrc;
    QList<GNodeInfo> mModuleFaultSrc;
    QList<GNodeInfo> mIduFaultSrc;
};

#endif // GTEMPLATE_H
