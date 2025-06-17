#include "taskman/connections/LocalPosixConnection.h"
#include "taskman/backends/LocalPosixBackend.h"

LocalPosixConnection::LocalPosixConnection()
    : ILocalConnection(LocalPosixBackend::instance()) {}

LocalPosixConnection::~LocalPosixConnection() = default;
