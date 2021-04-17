#ifndef GTEMPLATE_H
#define GTEMPLATE_H

#include <QtCore/qglobal.h>
#include "GNodeInfo.h"
#include <QDomDocument>
#include <QDomElement>
#include <QStandardItemModel>
#include <QHash>

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

    //
    QStandardItemModel *getSystemModel() const;

    //
    QStandardItemModel *getIduModel() const;

    //
    const QList<GNodeInfo>& getModuleInfoList() const;

    //解析
    void parse();

private:
    QString mName;///< 模板名
    QDomDocument mDoc;
    QList<GNodeInfo> mModuleInfo;
    QStandardItemModel *mSystemModel;
    QStandardItemModel *mIduModel;
    QHash<QStandardItem *, GNodeInfo> mItemToNode;
    QHash<GNodeInfo, QStandardItem *> mNodeToItem;
};

#endif // GTEMPLATE_H
