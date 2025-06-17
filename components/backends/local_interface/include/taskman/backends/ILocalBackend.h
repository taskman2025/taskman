#ifndef ILocalBackend_INCLUDED
#define ILocalBackend_INCLUDED

#include "taskman/backends/IBackend.h"

class ILocalBackend : public IBackend {
    Q_OBJECT

public:
    using IBackend::IBackend;
    virtual ~ILocalBackend() override;

protected:
    virtual qint64 getProcessTreeUpdateIntervalInSeconds() const override;
    virtual qint64 getSystemInformationUpdateIntervalInSeconds() const override;
    virtual qint64 getRefilterIntervalInSeconds() const override;
};

#endif // ILocalBackend_INCLUDED
