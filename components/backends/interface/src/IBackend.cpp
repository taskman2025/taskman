#include "taskman/backends/IBackend.h"
#include <QDateTime>

IBackend::IBackend(IPlatformRuntime& platformRuntime)
    : m_platformRuntime{platformRuntime} {}

IBackend::~IBackend() = default;

IPlatformRuntime& IBackend::getPlatformRuntime() const {
    return m_platformRuntime;
}

void IBackend::requestProcessTree(backend_request_id_t id, timestamp_t lastTimeFetched) {
    timestamp_t currentTime = QDateTime::currentSecsSinceEpoch();
    ThreadsafeSharedConstPointer<ProcessTree> saved = m_savedProcessTree.get();
    if (
        saved.get() != nullptr
        && currentTime - saved->timestamp < getProcessTreeUpdateIntervalInSeconds()
    ) {
        if (lastTimeFetched > 0 && lastTimeFetched == saved->timestamp) {
            doReplyProcessTree(id, {.errorCode = ResponseErrorCode::NONE}, {});
        } else {
            doReplyProcessTree(id, {.errorCode = ResponseErrorCode::NONE}, saved);
        }
    } else {
        QPointer<IBackend> safeThis{this};
        buildTree()
            .then([safeThis, id]() {
                if (!safeThis) {
                    return;
                }
                ThreadsafeSharedConstPointer<ProcessTree> result = safeThis->m_savedProcessTree.get();
                Q_ASSERT(result.get() != nullptr);
                safeThis->doReplyProcessTree(id, {.errorCode = ResponseErrorCode::NONE}, result);
            })
            .onFailed([safeThis, id](QException const& e) {
                if (!safeThis) {
                    return;
                }
                safeThis->m_savedProcessTree.set({});
                safeThis->doReplyProcessTree(id, {.errorCode = ResponseErrorCode::OTHER, .errorMessage = e.what()}, {});
            });
    }
}

void IBackend::doReplyProcessTree(backend_request_id_t id, ResponseError error, ThreadsafeSharedConstPointer<ProcessTree> tree) {
    emit replyProcessTree(id, error, tree);
}

void IBackend::requestSystemInformation(backend_request_id_t id) {
    try {
        SystemInformation s = {
            .machineType = m_platformRuntime.getMachineType(),
            .osKernelType = m_platformRuntime.getOSKernelType(),
            .osKernelVersion = m_platformRuntime.getOSKernelVersion(),
            .osFamilyName = m_platformRuntime.getOSFamilyName(),
            .osName = m_platformRuntime.getOSName(),
            .osVersion = m_platformRuntime.getOSVersion()
        };

        doReplySystemInformation(
            id,
            {
                .errorCode = ResponseErrorCode::NONE,
            },
            s
        );
    } catch (std::exception const& e) {
        doReplySystemInformation(
            id,
            {.errorCode = ResponseErrorCode::OTHER,
             .errorMessage = e.what()},
            {}
        );
    }
}

void IBackend::doReplySystemInformation(backend_request_id_t id, ResponseError error, SystemInformation s) {
    emit replySystemInformation(id, error, s);
}

void IBackend::requestProcessAction(
    backend_request_id_t id,
    process_action_id_t actionId,
    proc_id_t pid
) {
    // TODO: might have to push this into a worker thread?
    try {
        ActionResult result = m_platformRuntime.applyAction(actionId, pid);
        doReplyProcessAction(
            id,
            {.errorCode = ResponseErrorCode::NONE},
            result
        );
    } catch (std::exception const& e) {
        doReplyProcessAction(
            id,
            {.errorCode = ResponseErrorCode::OTHER,
             .errorMessage = e.what()},
            {}
        );
    }
}

void IBackend::doReplyProcessAction(
    backend_request_id_t id,
    ResponseError error,
    ActionResult result

) {
    emit replyProcessAction(id, error, result);
}

QFuture<void> IBackend::buildTree() {
    if (!m_updateTreeFuture.isRunning()) {
        QPointer safeThis{this};
        m_updateTreeFuture
            = IBackend::doBuildProcessTree(m_platformRuntime)
                  .then([safeThis](QFuture<ProcessTree> f) {
                      if (!safeThis || !f.isValid()) {
                          return;
                      }
                      ThreadsafeSharedConstPointer<ProcessTree> newTree{
                          new ProcessTree{f.takeResult()}
                      };
                      safeThis->m_savedProcessTree.set(newTree);
                  });
    }
    return m_updateTreeFuture;
}

QFuture<ProcessTree> IBackend::doBuildProcessTree(IPlatformRuntime& runtime) {
    return QtConcurrent::run([&runtime]() {
        IPlatformProfile const& profile = runtime.getPlatformProfile();
        proc_id_t const imaginaryRootId = profile.getImaginaryRootProcId();
        // TODO: selective fields
        auto const& selectedFields = profile.getProcessFields();

        field_mask_t fieldFlags = 0;
        for (auto const& field : selectedFields) {
            fieldFlags |= field.mask;
        }

        QScopedPointer<IProcessReader> reader{runtime.startReadingProcesses(fieldFlags)};

        ProcessTree newTree = {
            .pidToProcessDataMap = {},
            .totalNumProcs = 0,
            .timestamp = static_cast<timestamp_t>(QDateTime::currentSecsSinceEpoch())
        };
        QHash<proc_id_t, ProcessData>& newMap = newTree.pidToProcessDataMap;
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

            ++newTree.totalNumProcs;
            
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

        return newTree;
    });
}

void IBackend::requestCommunicationPractices(backend_request_id_t id) {
    emit replyCommunicationPractices(
        id,
        {
            .errorCode = ResponseErrorCode::NONE,
        },
        {.processUpdateIntervalInSeconds = getProcessTreeUpdateIntervalInSeconds(),
         .systemInformationUpdateIntervalInSeconds = getSystemInformationUpdateIntervalInSeconds(),
         .refilterIntervalInSeconds = getRefilterIntervalInSeconds()}
    );
}

void IBackend::requestProcessFiltering(
    backend_request_id_t id,
    QSet<proc_id_t> pidSet,
    QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
) {
    QMap<filter_type_id_t, QPair<FilterError, QString>> errors;
    for (auto it = filterArgsList.cbegin(); it != filterArgsList.cend(); ++it) {
        filter_type_id_t filterTypeId = it.key();
        QList<QList<QVariant>> const& argsList = it.value();
        QScopedPointer<IProcessFilter> filter;

        try {
            filter.reset(m_platformRuntime.createFilter(filterTypeId, argsList));
            QString errorMessage;
            FilterError err = filter->apply(pidSet, errorMessage);
            if (err != FilterError::NONE) {
                errors.insert(filterTypeId, {err, errorMessage});
            }
        } catch (std::exception const& e) {
            emit replyProcessFiltering(
                id,
                {.errorCode = ResponseErrorCode::OTHER,
                 .errorMessage = e.what()},
                {},
                {}
            );
            return;
        }
    }

    emit replyProcessFiltering(
        id,
        {
            .errorCode = ResponseErrorCode::NONE,
        },
        errors,
        pidSet
    );
}
