#ifndef GNODEINFO_H
#define GNODEINFO_H

#include <QtCore/qglobal.h>
#include <QString>
#include <QHash>
#include <QDebug>
class GNodeInfo
{
public:
    GNodeInfo();
    QString mName;
    QString mSuffix;
    QString mSrc;
    QString mDisplayValue;
    QString key() const;

    //支持QMap
    bool operator <(const GNodeInfo& other) const;
};
bool operator ==(const GNodeInfo& a, const GNodeInfo& b);
bool operator ==(const GNodeInfo& a, const QString& b);
bool operator ==(const QString& a, const GNodeInfo& b);
QDebug operator<<(QDebug debug, const GNodeInfo& c);

//支持QHash
inline uint qHash(const GNodeInfo& key, uint seed)
{
    return (qHash(key.key(), seed));
}


#endif // GNODEINFO_H
