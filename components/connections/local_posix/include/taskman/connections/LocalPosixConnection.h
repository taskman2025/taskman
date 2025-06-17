#ifndef LocalPosixConnection_INCLUDED
#define LocalPosixConnection_INCLUDED

#include "taskman/connections/ILocalConnection.h"

class LocalPosixConnection : public ILocalConnection {
    Q_OBJECT

public:
    LocalPosixConnection();
    virtual ~LocalPosixConnection();
};

#endif // LocalPosixConnection_INCLUDED
