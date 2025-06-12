#ifndef IProcessFilter_INCLUDED
#define IProcessFilter_INCLUDED

#include "taskman/common/types.h"
#include "taskman/platform/IProcessReader.h"
#include "taskman/platform/ProcessField.h"

class IProcessFilter {
public:
    virtual ~IProcessFilter() = 0;
    bool operator==(IProcessFilter const& other) const;
    bool isEqual(IProcessFilter const& other) const;
    virtual bool applyToCurrent(IProcessReader const& reader) const = 0;
    virtual bool isApplicable() const = 0;
    virtual field_mask_t getFilterType() const = 0;

protected:
    virtual bool doIsEqualForSameFilterTypeAndApplicable(IProcessFilter const& other) const = 0;
};

class NonApplicableProcessFilter : public IProcessFilter {
public:
    NonApplicableProcessFilter();
    virtual ~NonApplicableProcessFilter();
    virtual bool applyToCurrent(IProcessReader const& reader) const override { return true; }
    virtual bool isApplicable() const override { return false; }
    virtual field_mask_t getFilterType() const override { return 0; }

protected:
    virtual bool doIsEqualForSameFilterTypeAndApplicable(IProcessFilter const& other) const override {
        return true; // this function will never be called because the instance is already non-applicable
    }
};

#endif // IProcessFilter_INCLUDED
