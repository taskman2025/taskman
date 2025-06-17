#include "taskman/backends/LocalPosixBackend.h"
#include "taskman/platform_runtimes/PosixPlatformRuntime.h"

LocalPosixBackend& LocalPosixBackend::instance() {
    static LocalPosixBackend inst;
    static bool initialized;
    if (!initialized) {
        inst.initialize();
    }
    return inst;
}

LocalPosixBackend::LocalPosixBackend() : ILocalBackend(PosixPlatformRuntime::instance()) {}

LocalPosixBackend::~LocalPosixBackend() = default;
