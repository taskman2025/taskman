#ifndef PosixBaseProcessFilter_INCLUDED
#define PosixBaseProcessFilter_INCLUDED

#include "taskman/platform_runtimes/IProcessFilter.h"

class PosixBaseProcessFilter : public IProcessFilter {
public:
    using IProcessFilter::IProcessFilter;
    virtual ~PosixBaseProcessFilter() = 0;
};

#endif // PosixBaseProcessFilter_INCLUDED
