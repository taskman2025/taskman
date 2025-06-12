#ifndef IProcessReader_INCLUDED
#define IProcessReader_INCLUDED

#include "taskman/common/types.h"
#include "taskman/platform/ProcessField.h"
#include <QVariant>

class IProcessReader {
public:
    virtual ~IProcessReader() = 0;
    virtual proc_id_t getCurrentProcId() const = 0;
    virtual proc_id_t getCurrentProcPPID() const = 0;
    virtual QVariant getCurrentProcData(field_mask_t fieldBit) const = 0;
    inline bool next() { return doNext(); }
    inline bool isFinished() { return doIsFinished(); }

protected:
    virtual bool doNext() = 0;
    virtual bool doIsFinished() const = 0;
};

#endif // IProcessReader_INCLUDED
