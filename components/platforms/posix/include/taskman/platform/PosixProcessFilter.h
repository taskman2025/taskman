#ifndef PosixProcessFilter_INCLUDED
#define PosixProcessFilter_INCLUDED

#include "taskman/platform/IProcessFilter.h"
#include "taskman/platform/PosixCommonDefs.h"
#include <dirent.h>
#include <sys/stat.h>

class PosixPlatform;

class BasePosixProcessFilter : public IProcessFilter {
public:
    virtual ~BasePosixProcessFilter() override;
};

class PosixProcessOpenFileFilter : public BasePosixProcessFilter {
private:
    PosixPlatform const& m_platform;
    uint64_t m_targetFileDev;
    uint64_t m_targetFileInode;
    bool m_applicable;

public:
    PosixProcessOpenFileFilter(PosixPlatform const& platform, QString filePath);

    virtual ~PosixProcessOpenFileFilter() override;

    virtual bool applyToCurrent(IProcessReader const& reader) const override;

    virtual bool isApplicable() const override { return m_applicable; }

    virtual field_mask_t getFilterType() const override { return FilterBit::OPEN_FILE; }

protected:
    virtual bool doIsEqualForSameFilterTypeAndApplicable(IProcessFilter const& abstractOther) const override;
};

#endif // PosixProcessFilter_INCLUDED
