#ifndef PosixProcessFilter_INCLUDED
#define PosixProcessFilter_INCLUDED

#include "taskman/common/types.h"
#include "taskman/platform_runtimes/posix/process_filters/PosixBaseProcessFilter.h"

class PosixProcessFilter {
public:
    static PosixBaseProcessFilter* createInternalFilter(
        filter_type_id_t filterTypeId,
        QList<QList<QVariant>> const& argsList
    );
};

#endif // PosixProcessFilter_INCLUDED
