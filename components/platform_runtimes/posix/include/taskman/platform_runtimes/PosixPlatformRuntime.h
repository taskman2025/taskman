#ifndef PosixPlatformRuntime_INCLUDED
#define PosixPlatformRuntime_INCLUDED

#include "taskman/platform_runtimes/IPlatformRuntime.h"
#include <QCache>
#include <QString>
#include <QVariant>

class PosixPlatformRuntime : public IPlatformRuntime {
public:
    static PosixPlatformRuntime& instance();

    virtual ~PosixPlatformRuntime() override;

    virtual QString getMachineType() override;
    virtual QString getOSKernelVersion() override;
    virtual QString getOSFamilyName() override;
    virtual QString getOSName() override;
    virtual QString getOSVersion() override;

    virtual IProcessReader* startReadingProcesses(field_mask_t fields) override;
    virtual IProcessFilter* createFilter(filter_type_id_t filterType, QList<QList<QVariant>> const& argsList) override;
    virtual ActionResult applyAction(process_action_id_t actionId, proc_id_t pid) override;

    QString getUserNameById(uint64_t userId);
    QString getGroupNameById(uint64_t groupId);

private:
    PosixPlatformRuntime();
    QCache<uint64_t, QString> m_userNameCache;
    QCache<uint64_t, QString> m_groupNameCache;
    QString m_machineType;
    QString m_kernelVersion;
    QString m_osFamilyName;
    QString m_osName;
    QString m_osVersion;
};

#endif // PosixPlatformRuntime_INCLUDED
