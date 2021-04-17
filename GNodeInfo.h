#ifndef GNODEINFO_H
#define GNODEINFO_H

#include <QtCore/qglobal.h>

class GNodeInfo
{
public:
    GNodeInfo();
    QString mName;
    QString mSuffix;
    QString mSrc;
    QString mDisplayValue;
    //支持QMap
    bool operator <(const GNodeInfo& other) const;
};
//支持QHash
inline uint qHash(const GNodeInfo& key, uint seed)
{
    return (qHash(key.mName+key.mDisplayValue+key.mSrc+key.mSuffix, seed));
}


#endif // GNODEINFO_H
