#ifndef PosixProcessReader_INCLUDED
#define PosixProcessReader_INCLUDED

#include "taskman/platform_runtimes/IProcessReader.h"
#include "taskman/platform_runtimes/PosixPlatformRuntime.h"
#include <proc/readproc.h>

class PosixProcessReader : public IProcessReader {
public:
    PosixProcessReader(PosixPlatformRuntime& platformRuntime, field_mask_t fields);
    virtual ~PosixProcessReader() override;

protected:
    virtual bool doNext() override;
    virtual proc_id_t doGetCurrentPID() const override;
    virtual proc_id_t doGetCurrentPPID() const override;
    virtual QVariant doGetCurrentProcData(field_mask_t fieldBit) const override;

private:
    PosixPlatformRuntime& m_platformRuntime;
    PROCTAB* m_pt;
    proc_t m_procInfo;
};

#endif // PosixProcessReader_INCLUDED
