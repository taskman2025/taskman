#include "taskman/platform_runtimes/posix/PosixProcessReader.h"
#include "taskman/platform_profiles/posix/process_field_bits.h"

PosixProcessReader::PosixProcessReader(PosixPlatformRuntime& platformRuntime, field_mask_t fields)
    : m_platformRuntime{platformRuntime}, m_procInfo{0} {
    memset(&m_procInfo, 0, sizeof(proc_t));

    int flags = PROC_FILLSTAT;
    if (
        hasAny(
            fields,
            PosixProcessFieldBit::REAL_UID
                | PosixProcessFieldBit::EFFECTIVE_UID
                | PosixProcessFieldBit::REAL_GID
                | PosixProcessFieldBit::EFFECTIVE_GID
                | PosixProcessFieldBit::REAL_USER
                | PosixProcessFieldBit::EFFECTIVE_USER
                | PosixProcessFieldBit::REAL_GROUP
                | PosixProcessFieldBit::EFFECTIVE_GROUP
        )
    ) {
        flags |= PROC_FILLSTATUS;
    }

    if (hasAny(fields, PosixProcessFieldBit::COMMAND_LINE)) {
        flags |= PROC_FILLARG;
    }

    m_pt = openproc(flags);
}

PosixProcessReader::~PosixProcessReader() {
    closeproc(m_pt);
}

bool PosixProcessReader::doNext() {
    do {
        if (readproc(m_pt, &m_procInfo) == nullptr) {
            return false;
        }
    } while (m_procInfo.tid != m_procInfo.tgid || m_procInfo.tid <= 0 || m_procInfo.tgid <= 0 || m_procInfo.ppid < 0); // skip non-main threads
    return true;
}

proc_id_t PosixProcessReader::doGetCurrentPID() const {
    return static_cast<proc_id_t>(m_procInfo.tid);
}

proc_id_t PosixProcessReader::doGetCurrentPPID() const {
    return static_cast<proc_id_t>(m_procInfo.ppid);
}

QVariant PosixProcessReader::doGetCurrentProcData(field_mask_t fieldBit) const {
    switch (fieldBit) {
    case PosixProcessFieldBit::PID:
        return QVariant::fromValue(getCurrentPID());
    case PosixProcessFieldBit::PPID:
        return QVariant::fromValue(getCurrentPPID());
    case PosixProcessFieldBit::NAME:
        return m_procInfo.cmd ? m_procInfo.cmd : "N/A";
    case PosixProcessFieldBit::STATE:
        return QVariant::fromValue<char>(m_procInfo.state);
    case PosixProcessFieldBit::PGRP:
        return m_procInfo.pgrp;
    case PosixProcessFieldBit::NICE:
        return QVariant::fromValue(m_procInfo.nice);
    case PosixProcessFieldBit::STARTTIME:
        return m_procInfo.start_time;
    case PosixProcessFieldBit::REAL_UID:
        return m_procInfo.ruid;
    case PosixProcessFieldBit::EFFECTIVE_UID:
        return m_procInfo.euid;
    case PosixProcessFieldBit::REAL_GID:
        return m_procInfo.rgid;
    case PosixProcessFieldBit::EFFECTIVE_GID:
        return m_procInfo.egid;
    case PosixProcessFieldBit::REAL_USER:
        return m_platformRuntime.getUserNameById(m_procInfo.ruid);
        // return QString{m_procInfo.ruser};
    case PosixProcessFieldBit::EFFECTIVE_USER:
        return m_platformRuntime.getUserNameById(m_procInfo.egid);
        // return QString{m_procInfo.euser};
    case PosixProcessFieldBit::REAL_GROUP:
        return m_platformRuntime.getGroupNameById(m_procInfo.rgid);
        // return QString{m_procInfo.rgroup};
    case PosixProcessFieldBit::EFFECTIVE_GROUP:
        return m_platformRuntime.getGroupNameById(m_procInfo.egid);
        // return QString{m_procInfo.egroup};

    case PosixProcessFieldBit::COMMAND_LINE: {
        char** cmdlineArray = m_procInfo.cmdline;
        if (cmdlineArray == NULL)
            return "N/A";
        QStringList args;
        for (int i = 0; cmdlineArray[i] != NULL; ++i) {
            args << cmdlineArray[i];
        }
        return args.join(" ");
    }

    default:
        return "N/A";
    }
}
