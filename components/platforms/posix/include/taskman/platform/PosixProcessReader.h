#ifndef PosixProcessReader_INCLUDED
#define PosixProcessReader_INCLUDED

#include "taskman/platform/IProcessReader.h"
#include "taskman/platform/PosixCommonDefs.h"
#include "taskman/platform/ProcessField.h"
#include <QStringList>
#include <exception>
#include <proc/readproc.h>
#include <QCache>
#include "taskman/platform/PosixPlatform.h"
#include "taskman/common/ThreadsafeSharedConstPointer.h"

class PosixProcessReader : public IProcessReader {
private:
    PosixPlatform& m_platform;
    PROCTAB* m_pt;
    proc_t m_procInfo;
    bool m_finished;
    QList<ThreadsafeConstSharedPointer<IProcessFilter>> m_filters;

    void ensureNotFinished() const {
        if (m_finished) {
            throw std::runtime_error("no more processes to read");
        }
    }

public:
    PosixProcessReader(PosixPlatform& platform, field_mask_t fields, QList<ThreadsafeConstSharedPointer<IProcessFilter>> filters);
    virtual ~PosixProcessReader() override;
    virtual proc_id_t getCurrentProcId() const override {
        ensureNotFinished();
        return m_procInfo.tid;
    }
    virtual proc_id_t getCurrentProcPPID() const override {
        ensureNotFinished();
        return m_procInfo.ppid;
    }

    virtual QVariant getCurrentProcData(field_mask_t field) const override {
        switch (field) {
        case FieldBit::PID:
            return QVariant::fromValue(getCurrentProcId());
        case FieldBit::PPID:
            return QVariant::fromValue(getCurrentProcPPID());
        case FieldBit::NAME:
            return m_procInfo.cmd ? m_procInfo.cmd : "N/A";
        case FieldBit::STATE:
            return QVariant::fromValue<char>(m_procInfo.state);
        case FieldBit::PGRP:
            return m_procInfo.pgrp;
        case FieldBit::NICE:
            return QVariant::fromValue(m_procInfo.nice);
        case FieldBit::STARTTIME:
            return m_procInfo.start_time;
        case FieldBit::REAL_UID:
            return m_procInfo.ruid;
        case FieldBit::EFFECTIVE_UID:
            return m_procInfo.euid;
        case FieldBit::REAL_GID:
            return m_procInfo.rgid;
        case FieldBit::EFFECTIVE_GID:
            return m_procInfo.egid;
        case FieldBit::REAL_USER:
            return m_platform.getUserNameById(m_procInfo.ruid);
            // return QString{m_procInfo.ruser};
        case FieldBit::EFFECTIVE_USER:
            return m_platform.getUserNameById(m_procInfo.egid);
            // return QString{m_procInfo.euser};
        case FieldBit::REAL_GROUP:
            return m_platform.getGroupNameById(m_procInfo.rgid);
            // return QString{m_procInfo.rgroup};
        case FieldBit::EFFECTIVE_GROUP:
            return m_platform.getGroupNameById(m_procInfo.egid);
            // return QString{m_procInfo.egroup};
        case FieldBit::FILTERED: {
            for (auto filter : m_filters) {
                if (!filter->isApplicable()) continue;
                if (filter->applyToCurrent(*this)) {
                    return true;
                }
            }
            return false;
        }

        case FieldBit::COMMAND_LINE: {
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

protected:
    virtual bool doNext() override {
        for (;;) {
            m_finished = (readproc(m_pt, &m_procInfo) == nullptr);
            if (m_finished)
                break;
            if (m_procInfo.tid != m_procInfo.tgid) {
                continue; // skip non-main threads
            }
            break;
        }
        return !m_finished;
    }
    virtual bool doIsFinished() const override {
        return m_finished;
    }
};

#endif // PosixProcessReader_INCLUDED
