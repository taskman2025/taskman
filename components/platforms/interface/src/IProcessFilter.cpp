#include "taskman/platform/IProcessFilter.h"

IProcessFilter::~IProcessFilter() {}

bool IProcessFilter::operator==(IProcessFilter const& other) const {
    return isEqual(other);
}

bool IProcessFilter::isEqual(IProcessFilter const& other) const {
    if (!isApplicable() && !other.isApplicable()) {
        return true;
    }
    if (!isApplicable() || !other.isApplicable()) {
        return false;
    }
    if (getFilterType() != other.getFilterType()) {
        return false;
    }
    return doIsEqualForSameFilterTypeAndApplicable(other);
}

NonApplicableProcessFilter::NonApplicableProcessFilter() {}
NonApplicableProcessFilter::~NonApplicableProcessFilter() {}
