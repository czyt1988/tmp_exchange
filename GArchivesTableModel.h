#ifndef GARCHIVESTABLEMODEL_H
#define GARCHIVESTABLEMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QIcon>
#include <QColor>
class GArchivesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    class ArchiveInfo {
public:
        ArchiveInfo();
        ArchiveInfo(const QString& info);
        QString mInfo;
        QIcon mIcon;
        QColor mBackColor;
        QString mType;
    };

public:
    explicit GArchivesTableModel(QObject *parent = 0);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    //清空信息
    void clear();

    //插入信息
    void insertInfo(const QDateTime& dt, const ArchiveInfo& info);

    //刷新
    void update();

    //获取计数
    int count() const;

    //排序
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);


    //获取第i行
    QPair<QDateTime, ArchiveInfo> getAt(int i) const;

private:
    QList<QPair<QDateTime, ArchiveInfo> > m_archiveInfos;
};

#endif // GARCHIVESTABLEMODEL_H
