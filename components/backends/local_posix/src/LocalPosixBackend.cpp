#include "taskman/backends/LocalPosixBackend.h"
#include "taskman/platform_runtimes/PosixPlatformRuntime.h"

LocalPosixBackend& LocalPosixBackend::instance() {
    static LocalPosixBackend inst;
    return inst;
}

LocalPosixBackend::LocalPosixBackend() : ILocalBackend(PosixPlatformRuntime::instance()) {}

LocalPosixBackend::~LocalPosixBackend() = default;
