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

    //
    const QList<GNodeInfo>& getModuleInfoList() const;
    const QList<GNodeInfo>& getSystemInfoList() const;
    const QList<GNodeInfo>& getIduInfoList() const;

    //更新数据
    void updateValue(const GNodeInfo& value);

    const QString& getCanipField() const;

private:
    void parse(QDomDocument& doc);
    void loadsetting(QDomElement& root);
    void loadSystemInfo(QDomElement& root);
    bool loadSystemItemFromNode(QDomElement& nodeitem, QStandardItem *parItem);
    void loadIduInfo(QDomElement& root);
    bool loadIduItemFromNode(QDomElement& nodeitem);
    void loadModuleInfo(QDomElement& root);
    bool loadModuleItemFromNode(QDomElement& nodeitem);

private:
    QString mName;///< 模板名

    QList<GNodeInfo> mSystemInfo;
    QList<GNodeInfo> mModuleInfo;
    QList<GNodeInfo> mIduInfo;
    QStandardItemModel *mSystemModel;
    GIDUTableModel *mIduModel;
    QHash<QStandardItem *, GNodeInfo> mValueItemToNode;
    QHash<GNodeInfo, QStandardItem *> mNodeToItemValue;

    QString mCanipfield;
};

#endif // GTEMPLATE_H
