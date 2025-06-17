#ifndef PosixProcessActionId_INCLUDED
#define PosixProcessActionId_INCLUDED

#include "taskman/common/declaration.h"

class PosixProcessActionId {
public:
    DECLARE_PROCESS_ACTION_ID(END, 1);
    DECLARE_PROCESS_ACTION_ID(KILL, 2);
};

#endif // PosixProcessActionId_INCLUDED
