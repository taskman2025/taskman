#ifndef PosixPlatform_INCLUDED
#define PosixPlatform_INCLUDED

#include "taskman/platform/IPlatform.h"
#include "PosixProcessFilter.h"
#include <QList>
#include <QVariant>
#include <QCache>

class PosixPlatform : public IPlatform {
public:
    PosixPlatform();
    
    virtual QList<ProcessField> const& getProcessFields() const override;

    virtual QString const& getPlatformName() override;

    virtual QString const& getOSFamilyName() override;
    virtual QString const& getOSName() override;
    virtual QString const& getOSVersion() override;

    virtual IProcessReader* startReadingProcesses(ReadProcessesRequest req) override;
    virtual IProcessFilter* createProcessFilter(field_mask_t filterBy, QVariant arg) override;

    QString getUserNameById(uint64_t userId);
    QString getGroupNameById(uint64_t groupId);

private:
    static QList<ProcessField> const PROCESS_FIELDS;
    static QString const PLATFORM_NAME;

    QString m_osFamilyName;
    QString m_osName;
    QString m_osVersion;

    QCache<uint64_t, QString> m_userCache;
    QCache<uint64_t, QString> m_groupCache;
};

#endif // PosixPlatform_INCLUDED
