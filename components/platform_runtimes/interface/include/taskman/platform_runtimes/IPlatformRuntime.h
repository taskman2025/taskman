#ifndef IPlatformRuntime_INCLUDED
#define IPlatformRuntime_INCLUDED

#include "taskman/platform_profiles/ProcessField.h"
#include "taskman/platform_profiles/IPlatformProfile.h"
#include "taskman/platform_runtimes/IProcessReader.h"
#include "taskman/platform_runtimes/IProcessFilter.h"

#include <QList>
#include <QVariant>

enum class ActionErrorCode {
    NONE,
    OTHER,
    NO_SUCH_ACTION,
    NO_SUCH_PROCESS,
    PERMISSION_DENIED,
};

struct ActionResult {
    ActionErrorCode errorCode;
    QString message;
};

class IPlatformRuntime {
public:
    IPlatformRuntime(IPlatformProfile const& platformProfile);
    virtual ~IPlatformRuntime() = 0;
    virtual QString getMachineType();  // x86-64, amd64, arm64, aarch64, x86...
    virtual QString getOSKernelType(); // winnt, darwin, linux...
    virtual QString getOSKernelVersion();
    virtual QString getOSFamilyName() = 0; // Linux, Darwin, FreeBSD, Windows...
    virtual QString getOSName();           // Ubuntu, Linux Mint, Windows...
    virtual QString getOSVersion();        // 22.04, 21.3 (Virginia), 11...

    /**
     * The returned IProcessReader* pointer is owned by the caller.
     * It must be used once only, no reuse.
     */
    virtual IProcessReader* startReadingProcesses(field_mask_t fields) = 0;

    /**
     * The returned IProcessFilter* pointer is owned by the caller.
     * It can and should always be reused whenever possible.
     */
    virtual IProcessFilter* createFilter(filter_type_id_t filterType, QList<QList<QVariant>> const& argsList) = 0;

    virtual ActionResult applyAction(process_action_id_t actionId, proc_id_t pid) = 0;

    IPlatformProfile const& getPlatformProfile() const;

private:
    IPlatformProfile const& m_platformProfile;
};

#endif // IPlatformRuntime_INCLUDED
