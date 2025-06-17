#include "taskman/backends/IBackend.h"
#include <QDateTime>
#include <thread>
#include <chrono>

IBackend::IBackend(IPlatformRuntime& platformRuntime)
    : m_platformRuntime{platformRuntime}, m_procTreeBuilder{nullptr} {}

IBackend::~IBackend() {
    m_treeBuildingThread.quit();
    m_treeBuildingThread.wait();
    delete m_procTreeBuilder;
}

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
        // Need to rebuild. But first, just return the old tree!
        doReplyProcessTree(id, {.errorCode = ResponseErrorCode::NONE}, saved);
        emit buildTreeNow();
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

void IBackend::initialize() {
    m_procTreeBuilder = new ProcessTreeBuilder(m_platformRuntime);
    connect(this, &IBackend::buildTreeNow, m_procTreeBuilder, &ProcessTreeBuilder::build);
    connect(m_procTreeBuilder, &ProcessTreeBuilder::buildFinished, this, &IBackend::onTreeBuilt);
    m_procTreeBuilder->moveToThread(&m_treeBuildingThread);
    m_treeBuildingThread.start();
}

void IBackend::onTreeBuilt(ThreadsafeSharedConstPointer<ProcessTree> tree) {
    m_savedProcessTree.set(tree);
}
