#include "GNodeInfo.h"

GNodeInfo::GNodeInfo()
{
}


QString GNodeInfo::key() const
{
    return (mName+"/"+mSrc+"/"+mSuffix);
}


bool GNodeInfo::operator <(const GNodeInfo& other) const
{
    return ((this->mDisplayValue < other.mDisplayValue) &&
           (this->mName < other.mName) &&
           (this->mSrc < other.mSrc) &&
           (this->mSuffix < other.mSuffix));
}


bool operator ==(const GNodeInfo& a, const GNodeInfo& b)
{
    return (a.key() == b.key());
}


bool operator ==(const GNodeInfo& a, const QString& b)
{
    return (a.key() == b);
}


bool operator ==(const QString& a, const GNodeInfo& b)
{
    return (a == b.key());
}


QDebug operator<<(QDebug debug, const GNodeInfo& c)
{
    QDebugStateSaver saver(debug);

    debug.nospace() << "(" << c.mName << ")[" << c.mSrc << "][" << c.mSuffix << "] = " << c.mDisplayValue;
    return (debug);
}
