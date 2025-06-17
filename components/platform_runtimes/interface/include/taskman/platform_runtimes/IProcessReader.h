#ifndef IProcessReader_INCLUDED
#define IProcessReader_INCLUDED

#include "taskman/common/types.h"
#include <QVariant>

class IProcessReader {
public:
    IProcessReader();
    virtual ~IProcessReader() = 0;
    bool next();
    bool isFinished() const;
    virtual proc_id_t getCurrentPID() const;
    virtual proc_id_t getCurrentPPID() const;
    virtual QVariant getCurrentProcData(field_mask_t fieldBit) const;

protected:
    virtual bool doNext() = 0;
    virtual proc_id_t doGetCurrentPID() const = 0;
    virtual proc_id_t doGetCurrentPPID() const = 0;
    virtual QVariant doGetCurrentProcData(field_mask_t fieldBit) const = 0;

private:
    bool m_finished;
    bool m_firstTime;
};

#endif // IProcessReader_INCLUDED
