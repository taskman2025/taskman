#ifndef PosixActionRunner_INCLUDED
#define PosixActionRunner_INCLUDED

#include "taskman/platform_runtimes/IPlatformRuntime.h"

class PosixPlatformRuntime;

class PosixActionRunner {
public:
    PosixActionRunner(PosixPlatformRuntime& runtime);
    ActionResult run(process_action_id_t actionId, proc_id_t pid);

private:
    PosixPlatformRuntime& m_runtime;

    ActionResult sendSignal(proc_id_t pid, int signal);
};

#endif // PosixActionRunner_INCLUDED
