#ifndef IPlatform_INCLUDED
#define IPlatform_INCLUDED

#include "taskman/common/ThreadsafeSharedConstPointer.h"
#include "taskman/common/types.h"
#include "taskman/platform/IProcessFilter.h"
#include "taskman/platform/IProcessReader.h"
#include <QString>
#include "taskman/common/ThreadsafeSharedConstPointer.h"

struct ReadProcessesRequest {
    field_mask_t fields;
    QList<ThreadsafeConstSharedPointer<IProcessFilter>> filters;
};

class IPlatform {
public:
    virtual QList<ProcessField> const& getProcessFields() const = 0;
    virtual QString const& getPlatformName() = 0;
    virtual QString const& getOSFamilyName() = 0;
    virtual QString const& getOSName() = 0;
    virtual QString const& getOSVersion() = 0;

    virtual IProcessReader* startReadingProcesses(ReadProcessesRequest req) = 0;
    virtual IProcessFilter* createProcessFilter(field_mask_t filterBy, QVariant arg) = 0;
    // TODO: Get more system information
};

#endif // IPlatform_INCLUDED
