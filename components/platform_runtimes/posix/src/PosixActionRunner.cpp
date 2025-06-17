#include "taskman/platform_runtimes/posix/PosixActionRunner.h"
#include "taskman/platform_profiles/posix/process_action_ids.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

PosixActionRunner::PosixActionRunner(PosixPlatformRuntime& runtime)
    : m_runtime{runtime} {
}

ActionResult PosixActionRunner::run(
    process_action_id_t actionId,
    proc_id_t pid
) {
    switch (actionId) {
    case PosixProcessActionId::END:
        return sendSignal(pid, SIGTERM);

    case PosixProcessActionId::KILL:
        return sendSignal(pid, SIGKILL);

    default:
        return {
            .errorCode = ActionErrorCode::NO_SUCH_ACTION,
            .message = ""
        };
    }
}

ActionResult PosixActionRunner::sendSignal(proc_id_t pid, int signal) {
    pid_t nativePid = static_cast<pid_t>(pid);
    int result = kill(pid, signal);
    int e = errno;
    if (result == 0) {
        return {
            .errorCode = ActionErrorCode::NONE,
            .message = QString("Signal sent to process %1").arg(nativePid)
        };
    } else {
        if (e == EPERM) {
            return {
                .errorCode = ActionErrorCode::PERMISSION_DENIED,
                .message = ""
            };
        } else if (e == ESRCH) {
            return {
                .errorCode = ActionErrorCode::NO_SUCH_PROCESS,
                .message = ""
            };
        } else {
            return {
                .errorCode = ActionErrorCode::OTHER,
                .message = QString("errno = %1").arg(e)
            };
        }
    }
}
