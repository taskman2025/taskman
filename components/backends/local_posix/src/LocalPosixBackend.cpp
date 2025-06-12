#include "taskman/backend/local_posix/LocalPosixBackend.h"
#include "taskman/backend/local_posix/PosixProcessItemModel.h"
#include "taskman/backend/local_posix/LocalPosixConnectionTab.h"

LocalPosixBackend::LocalPosixBackend() {
    m_platform.reset(new PosixPlatform());
}

IProcessItemModel* LocalPosixBackend::createProcessItemModel() {
    return new PosixProcessItemModel(*m_platform);
}

QSharedPointer<IPlatform> LocalPosixBackend::getPlatform() {
    return m_platform;
}

IConnectionTab* LocalPosixBackend::createConnectionTab() {
    return new LocalPosixConnectionTab(this);
}
