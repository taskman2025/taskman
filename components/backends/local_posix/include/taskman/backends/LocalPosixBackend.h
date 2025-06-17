#ifndef LocalPosixBackend_INCLUDED
#define LocalPosixBackend_INCLUDED

#include "taskman/backends/ILocalBackend.h"

class LocalPosixBackend : public ILocalBackend {
    Q_OBJECT

public:
    static LocalPosixBackend& instance();
    virtual ~LocalPosixBackend() override;

private:
    LocalPosixBackend();
};

#endif // LocalPosixBackend_INCLUDED
