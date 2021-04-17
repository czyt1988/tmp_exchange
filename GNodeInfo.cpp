#include "GNodeInfo.h"

GNodeInfo::GNodeInfo()
{
}


bool GNodeInfo::operator <(const GNodeInfo& other) const
{
    return ((this->mDisplayValue < other.mDisplayValue) &&
           (this->mName < other.mName) &&
           (this->mSrc < other.mSrc) &&
           (this->mSuffix < other.mSuffix));
}
