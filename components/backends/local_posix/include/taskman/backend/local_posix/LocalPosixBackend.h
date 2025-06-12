#ifndef LocalLinuxBackend_INCLUDED
#define LocalLinuxBackend_INCLUDED

#include "taskman/backend/IBackend.h"
#include "taskman/platform/PosixPlatform.h"
#include "taskman/backend/local_posix/PosixProcessItemModel.h"
#include <QSharedPointer>

class LocalPosixBackend : public IBackend {
public:
    LocalPosixBackend();
    virtual IProcessItemModel* createProcessItemModel() override;
    virtual QSharedPointer<IPlatform> getPlatform() override;
    virtual IConnectionTab* createConnectionTab() override;

private:
    QSharedPointer<PosixPlatform> m_platform;
};

#endif // LocalLinuxBackend_INCLUDED
