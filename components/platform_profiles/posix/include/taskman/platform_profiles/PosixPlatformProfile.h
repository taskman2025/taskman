#ifndef PosixPlatformProfile_INCLUDED
#define PosixPlatformProfile_INCLUDED

#include "taskman/platform_profiles/IPlatformProfile.h"
#include <QList>

class PosixPlatformProfile : public IPlatformProfile {
public:
    static PosixPlatformProfile& instance();

    virtual ~PosixPlatformProfile() override;
    virtual QList<ProcessField> const& getProcessFields() const override;
    virtual QList<ProcessFilterType> const& getProcessFilterTypes() const override;
    virtual QList<ProcessAction> const& getProcessActions() const override;
    virtual proc_id_t getImaginaryRootProcId() const override;
    virtual void validateIds(proc_id_t& pid, proc_id_t& ppid, bool& ok) const override;

protected:
    PosixPlatformProfile();

private:
    static QList<ProcessField> const PROCESS_FIELDS;
    static QList<ProcessFilterType> const PROCESS_FILTER_TYPES;
    static QList<ProcessAction> const PROCESS_ACTIONS;
};

#endif // PosixPlatformProfile_INCLUDED
