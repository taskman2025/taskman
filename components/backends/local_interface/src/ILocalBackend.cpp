#include "taskman/backends/ILocalBackend.h"

ILocalBackend::~ILocalBackend() = default;

qint64 ILocalBackend::getProcessTreeUpdateIntervalInSeconds() const {
    return 1;
}

qint64 ILocalBackend::getSystemInformationUpdateIntervalInSeconds() const {
    return 15;
}

qint64 ILocalBackend::getRefilterIntervalInSeconds() const {
    return 5;
}
