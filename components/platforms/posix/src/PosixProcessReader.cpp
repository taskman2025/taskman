#include "taskman/platform/PosixProcessReader.h"
#include "taskman/platform/PosixCommonDefs.h"

PosixProcessReader::PosixProcessReader(PosixPlatform& platform, field_mask_t fields, QList<ThreadsafeConstSharedPointer<IProcessFilter>> filters)
    : m_platform{platform}, m_procInfo{}, m_filters{filters}, m_finished{true} {
    int flags = PROC_FILLSTAT;
    if (
        hasAny(
            fields,
            FieldBit::REAL_UID | FieldBit::EFFECTIVE_UID | FieldBit::REAL_GID | FieldBit::EFFECTIVE_GID | FieldBit::REAL_USER | FieldBit::EFFECTIVE_USER | FieldBit::REAL_GROUP | FieldBit::EFFECTIVE_GROUP
        )
    ) {
        flags |= PROC_FILLSTATUS;
    }
    if (hasAny(fields, FieldBit::COMMAND_LINE)) {
        flags |= PROC_FILLARG;
    }
    m_pt = openproc(flags);
}

PosixProcessReader::~PosixProcessReader() {
    closeproc(m_pt);
}
