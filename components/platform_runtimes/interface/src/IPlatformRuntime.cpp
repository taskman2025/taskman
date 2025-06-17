#include "taskman/platform_runtimes/IPlatformRuntime.h"
#include <QSysInfo>

IPlatformRuntime::IPlatformRuntime(IPlatformProfile const& platformProfile)
    : m_platformProfile{platformProfile} {
}

IPlatformRuntime::~IPlatformRuntime() = default;

QString IPlatformRuntime::getMachineType() {
    return QSysInfo::currentCpuArchitecture();
}

QString IPlatformRuntime::getOSKernelType() {
    return QSysInfo::kernelType();
}

QString IPlatformRuntime::getOSKernelVersion() {
    return QSysInfo::kernelVersion();
}

QString IPlatformRuntime::getOSName() {
    return QSysInfo::productType();
}

QString IPlatformRuntime::getOSVersion() {
    return QSysInfo::productVersion();
}

IPlatformProfile const& IPlatformRuntime::getPlatformProfile() const {
    return m_platformProfile;
}
