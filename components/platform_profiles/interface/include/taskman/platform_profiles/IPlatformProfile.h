#ifndef IPlatformProfile_INCLUDED
#define IPlatformProfile_INCLUDED

#include "taskman/platform_profiles/ProcessField.h"
#include "taskman/platform_profiles/ProcessFilterType.h"
#include "taskman/platform_profiles/ProcessAction.h"
#include <QList>
#include <QString>

class IPlatformProfile {
public:
    virtual ~IPlatformProfile() = 0;
    virtual QList<ProcessField> const& getProcessFields() const = 0;
    virtual QList<ProcessFilterType> const& getProcessFilterTypes() const = 0;
    virtual QList<ProcessAction> const& getProcessActions() const = 0;

    /**
     * Returns the imaginary parent process ID of all processes,
     * even init(pid=1) on Linux.
     * 
     * This is simply to choose a process ID that will never be
     * there in the particular platform!
     * 
     * Choose a plausible value for the platform - e.g. -1 on Linux.
     * In that case, ProcessData struct object representing the "init" process
     * must have .pid = 1 and .ppid = -1.
     */
    virtual proc_id_t getImaginaryRootProcId() const = 0;

    /**
     * Validates PID and PPID of any process.
     * If any of them is invalid and there's no way to fix, set ok = false.
     * If any of them is invalid but there's a way to fix,
     *      fix them by setting new values to pid and ppid, then set ok = true.
     * If all of them is valid, simply set ok = true.
     */
    virtual void validateIds(proc_id_t& pid, proc_id_t& ppid, bool& ok) const = 0;
};

#endif // IPlatformProfile_INCLUDED
