#include "taskman/connections/ILocalConnection.h"
#include "taskman/common/concurrent.h"
#include <QMutex>
#include <QMutexLocker>

backend_request_id_t ILocalConnection::getNextId() {
    static QMutex mutex;
    static backend_request_id_t lastId = 0;
    QMutexLocker guard{&mutex};
    return ++lastId;
}

ILocalConnection::ILocalConnection(ILocalBackend& backend)
    : m_backend{backend}, m_id{getNextId()} {
}

ILocalConnection::~ILocalConnection() = default;

IPlatformProfile const& ILocalConnection::getPlatformProfile() const {
    return m_backend.getPlatformRuntime().getPlatformProfile();
}

void ILocalConnection::doInitialize() {
    {
        // REQUESTS/RESPONSES TO/FROM BACKEND
        connect(
            this,
            &ILocalConnection::i_requestCommunicationPractices,
            &m_backend,
            &ILocalBackend::requestCommunicationPractices
        );
        connect(
            &m_backend,
            &ILocalBackend::replyCommunicationPractices,
            this,
            &ILocalConnection::i_replyCommunicationPractices
        );

        connect(
            this,
            &ILocalConnection::i_requestSystemInformation,
            &m_backend,
            &ILocalBackend::requestSystemInformation
        );
        connect(
            &m_backend,
            &ILocalBackend::replySystemInformation,
            this,
            &ILocalConnection::i_replySystemInformation
        );

        connect(
            this,
            &ILocalConnection::i_requestProcessTree,
            &m_backend,
            &ILocalBackend::requestProcessTree
        );
        connect(
            &m_backend,
            &ILocalBackend::replyProcessTree,
            this,
            &ILocalConnection::i_replyProcessTree
        );

        connect(
            this,
            &ILocalConnection::i_requestProcessAction,
            &m_backend,
            &ILocalBackend::requestProcessAction
        );
        connect(
            &m_backend,
            &ILocalBackend::replyProcessAction,
            this,
            &ILocalConnection::i_replyProcessAction
        );

        connect(
            this,
            &ILocalConnection::i_requestProcessFiltering,
            &m_backend,
            &ILocalBackend::requestProcessFiltering
        );
        connect(
            &m_backend,
            &ILocalBackend::replyProcessFiltering,
            this,
            &ILocalConnection::i_replyProcessFiltering
        );
    }

    {
        // INTERNAL ROUTING
    }

    emit initialized();
}

#define CHECK_ID      \
    if (id != m_id) { \
        return;       \
    }

////////////////////////////////////////////////////

void ILocalConnection::doInitiateConnection() {
    emit i_requestCommunicationPractices(m_id);
}
void ILocalConnection::i_replyCommunicationPractices(
    backend_request_id_t id,
    ResponseError error,
    CommunicationPractices practices
) {
    CHECK_ID
    if (error.errorCode == ResponseErrorCode::NONE) {
        emit connectionInitiated(practices);
    } else {
        emit connectionInitiationError(
            {.errorCode = ConnectionErrorCode::BACKEND_FAILURE,
             .resErrorCode = error.errorCode,
             .errorMessage = error.errorMessage}
        );
    }
}

////////////////////////////////////////////////////

void ILocalConnection::doRequestProcessTree(timestamp_t lastTreeTimestamp) {
    emit i_requestProcessTree(m_id, lastTreeTimestamp);
}
void ILocalConnection::i_replyProcessTree(
    backend_request_id_t id,
    ResponseError error,
    ThreadsafeSharedConstPointer<ProcessTree> tree
) {
    CHECK_ID
    if (error.errorCode == ResponseErrorCode::NONE) {
        emit replyProcessTree(tree);
    } else {
        emit replyProcessTreeError(
            {.errorCode = ConnectionErrorCode::BACKEND_FAILURE,
             .resErrorCode = error.errorCode,
             .errorMessage = error.errorMessage}
        );
    }
}

////////////////////////////////////////////////////

void ILocalConnection::doRequestSystemInformation() {
    emit i_requestSystemInformation(m_id);
}
void ILocalConnection::i_replySystemInformation(
    backend_request_id_t id,
    ResponseError error, SystemInformation s
) {
    CHECK_ID
    if (error.errorCode == ResponseErrorCode::NONE) {
        emit replySystemInformation(s);
    } else {
        emit replySystemInformationError(
            {.errorCode = ConnectionErrorCode::BACKEND_FAILURE,
             .resErrorCode = error.errorCode,
             .errorMessage = error.errorMessage}
        );
    }
}

void ILocalConnection::doRequestProcessAction(
    process_action_id_t actionId,
    proc_id_t pid
) {
    emit i_requestProcessAction(m_id, actionId, pid);
}
void ILocalConnection::i_replyProcessAction(
    backend_request_id_t id,
    ResponseError error,
    ActionResult result
) {
    CHECK_ID
    if (error.errorCode == ResponseErrorCode::NONE) {
        emit replyProcessAction(result);
    } else {
        emit replyProcessActionError(
            {.errorCode = ConnectionErrorCode::BACKEND_FAILURE,
             .resErrorCode = error.errorCode,
             .errorMessage = error.errorMessage}
        );
    }
}

////////////////////////////////////////////////////

void ILocalConnection::doRequestProcessFiltering(
    QSet<proc_id_t> pidSet,
    QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
) {
    emit i_requestProcessFiltering(m_id, pidSet, filterArgsList);
}
void ILocalConnection::i_replyProcessFiltering(
    backend_request_id_t id,
    ResponseError error,
    QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
    QSet<proc_id_t> filteredPidSet
) {
    CHECK_ID
    if (error.errorCode == ResponseErrorCode::NONE) {
        emit replyProcessFiltering(errors, filteredPidSet);
    } else {
        emit replyProcessFilteringError(
            {.errorCode = ConnectionErrorCode::BACKEND_FAILURE,
             .resErrorCode = error.errorCode,
             .errorMessage = error.errorMessage}
        );
    }
}

#undef CHECK_ID
