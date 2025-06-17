#include "taskman/backends/ProcessTreeBuilder.h"
#include <QMutexLocker>

ProcessTreeBuilder::ProcessTreeBuilder(IPlatformRuntime& runtime)
    : m_runtime{runtime} {
}

ProcessTreeBuilder::~ProcessTreeBuilder() = default;

void ProcessTreeBuilder::build() {
    if (!m_buildMutex.tryLock()) {
        return;
    }
    try {
        IPlatformRuntime& runtime = m_runtime;
        IPlatformProfile const& profile = runtime.getPlatformProfile();
        proc_id_t const imaginaryRootId = profile.getImaginaryRootProcId();
        // TODO: selective fields
        auto const& selectedFields = profile.getProcessFields();

        field_mask_t fieldFlags = 0;
        for (auto const& field : selectedFields) {
            fieldFlags |= field.mask;
        }

        QScopedPointer<IProcessReader> reader{runtime.startReadingProcesses(fieldFlags)};

        ProcessTree* newTreePtr = new ProcessTree({.pidToProcessDataMap = {}, .totalNumProcs = 0, .timestamp = static_cast<timestamp_t>(QDateTime::currentSecsSinceEpoch())});
        QHash<proc_id_t, ProcessData>& newMap = newTreePtr->pidToProcessDataMap;
        ProcessData proc;

        while (reader->next()) {
            proc_id_t pid = reader->getCurrentPID();
            proc_id_t ppid = reader->getCurrentPPID();

            proc.setPID(pid);
            proc.setPPID(ppid);
            for (auto const& field : selectedFields) {
                proc.setFieldValue(field.mask, reader->getCurrentProcData(field.mask));
            }

            bool ok;
            profile.validateIds(pid, ppid, ok);
            if (!ok) {
                continue;
            }
            proc.setPID(pid);
            proc.setPPID(ppid);

            // Defensive sanity check (skip pathological entries)
            if (pid != imaginaryRootId && pid == ppid) {
                qWarning() << "process " << pid << " has itself as parent, skipping";
                continue;
            }

            ++newTreePtr->totalNumProcs;

            if (!newMap.contains(pid)) {
                newMap.insert(pid, proc);
            } else {
                newMap[pid].update(proc);
            }

            if (!newMap.contains(ppid)) {
                ProcessData dummyParent;
                dummyParent.setPID(ppid);
                dummyParent.setPPID(imaginaryRootId);
                dummyParent.addChildProcId(pid);
                newMap.insert(ppid, dummyParent);
            } else {
                newMap[ppid].addChildProcId(pid);
            }
        }

        emit buildFinished(ThreadsafeSharedConstPointer<ProcessTree>{newTreePtr});
        m_buildMutex.unlock();
    } catch (std::exception const&) {
        m_buildMutex.unlock();
        throw;
    }
}

bool ProcessTreeBuilder::isBuilding() {
    if (m_buildMutex.tryLock()) {
        m_buildMutex.unlock();
        return false;
    }
    return true;
}
