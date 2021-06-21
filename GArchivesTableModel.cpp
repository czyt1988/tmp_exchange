#include "GArchivesTableModel.h"
#include <QPalette>

GArchivesTableModel::ArchiveInfo::ArchiveInfo()
{
    QPalette pt;

    mBackColor = pt.color(QPalette::Window);
}


GArchivesTableModel::ArchiveInfo::ArchiveInfo(const QString& info) : mInfo(info)
{
    QPalette pt;

    mBackColor = pt.color(QPalette::Window);
}


GArchivesTableModel::GArchivesTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}


QVariant GArchivesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return (QStringLiteral("时间"));

                break;

            case 1:
                return (QStringLiteral("类别"));

                break;

            case 2:
                return (QStringLiteral("事件"));

                break;

            default:
                break;
            }
        }else{
            if (section < m_archiveInfos.size()) {
                return (QString::number(section+1));
            }
        }
    }
    return (QVariant());
}


int GArchivesTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return (3);
}


int GArchivesTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return (count());
}


QVariant GArchivesTableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.row() < m_archiveInfos.size()) {
            auto i = m_archiveInfos.begin();
            i += index.row();
            switch (index.column())
            {
            case 0:
                return (i->first.toString("yyyy-MM-dd HH:mm:ss"));

                break;

            case 1:
                return (i->second.mType);

                break;

            case 2:
                return (i->second.mInfo);

                break;

            default:
                break;
            }
        }
        return (QVariant());
    }else if (role == Qt::BackgroundRole) {
        if (index.row() < m_archiveInfos.size()) {
            auto i = m_archiveInfos.begin();
            i += index.row();
            QColor clr = i->second.mBackColor;
            if (clr.isValid()) {
                return (clr);
            }
        }
    }else if (role == Qt::DecorationRole) {
        if ((index.row() < m_archiveInfos.size()) && (1 == index.column())) {
            auto i = m_archiveInfos.begin();
            i += index.row();
            QIcon ico = i->second.mIcon;
            if (!ico.isNull()) {
                return (ico);
            }
        }
    }
    return (QVariant());
}


void GArchivesTableModel::clear()
{
    beginResetModel();
    m_archiveInfos.clear();
    endResetModel();
}


void GArchivesTableModel::insertInfo(const QDateTime& dt, const GArchivesTableModel::ArchiveInfo& info)
{
    m_archiveInfos.append(qMakePair(dt, info));
}


void GArchivesTableModel::update()
{
    beginResetModel();
    endResetModel();
}


int GArchivesTableModel::count() const
{
    return (m_archiveInfos.size());
}


void GArchivesTableModel::sort(int column, Qt::SortOrder order)
{
    switch (column)
    {
    case 0:
        beginResetModel();
        std::sort(m_archiveInfos.begin(), m_archiveInfos.end(), [order](const QPair<QDateTime, ArchiveInfo>& a, const QPair<QDateTime, ArchiveInfo>& b)->bool {
            if (order == Qt::AscendingOrder) {
                return (a.first < b.first);
            }
            return (a.first > b.first);
        });
        endResetModel();
        break;

    case 1:
        beginResetModel();
        std::sort(m_archiveInfos.begin(), m_archiveInfos.end(), [order](const QPair<QDateTime, ArchiveInfo>& a, const QPair<QDateTime, ArchiveInfo>& b)->bool {
            if (order == Qt::AscendingOrder) {
                return (a.second.mType < b.second.mType);
            }
            return (a.second.mType > b.second.mType);
        });
        endResetModel();

    case 2:
        beginResetModel();
        std::sort(m_archiveInfos.begin(), m_archiveInfos.end(), [order](const QPair<QDateTime, ArchiveInfo>& a, const QPair<QDateTime, ArchiveInfo>& b)->bool {
            if (order == Qt::AscendingOrder) {
                return (a.second.mInfo < b.second.mInfo);
            }
            return (a.second.mInfo > b.second.mInfo);
        });
        endResetModel();

    default:
        break;
    }
}


QPair<QDateTime, GArchivesTableModel::ArchiveInfo> GArchivesTableModel::getAt(int i) const
{
    if (i >= count()) {
        return (QPair<QDateTime, GArchivesTableModel::ArchiveInfo>());
    }
    return (m_archiveInfos.at(i));
}
