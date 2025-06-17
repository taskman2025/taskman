#include "taskman/connections/IConnection.h"
#include <QMutexLocker>

IConnection::IConnection()
    : m_systemInformationStaleTimer{},
      m_processTreeStaleTimer{},
      m_processFilterResultStaleTimer{},
      m_lastProcessTreeTimestamp{0},
      m_initialized{false},
      m_connected{false},
      m_filtering{false} {
}

IConnection::~IConnection() {
    m_systemInformationStaleTimer.stop();
    m_processTreeStaleTimer.stop();
    m_processFilterResultStaleTimer.stop();
}

bool IConnection::isConnected() const {
    return m_connected;
}

void IConnection::initiateConnection() {
    if (!m_initialized) {
        m_initialized = true;
        connect(
            &m_processTreeStaleTimer,
            &QTimer::timeout,
            this,
            &IConnection::requestProcessTree
        );

        connect(
            &m_systemInformationStaleTimer,
            &QTimer::timeout,
            this,
            &IConnection::requestSystemInformation
        );

        connect(
            &m_processFilterResultStaleTimer,
            &QTimer::timeout,
            this,
            &IConnection::refilterProcessesNow
        );

        connect(
            this,
            &IConnection::replyProcessTree,
            this,
            &IConnection::cacheLastProcessTreeTimestampAsNeeded
        );

        connect(
            this,
            &IConnection::replyProcessFiltering,
            this,
            &IConnection::onReplyProcessFiltering
        );

        connect(
            this,
            &IConnection::initialized,
            this,
            &IConnection::initiateConnection
        );

        connect(
            this,
            &IConnection::connectionInitiated,
            this,
            &IConnection::onInitiateConnectionSuccess
        );

        connect(
            this,
            &IConnection::connectionInitiationError,
            this,
            &IConnection::onInitiateConnectionError
        );

        doInitialize();
    } else {
        // initialized, can initate connection now
        if (!m_connected) {
            doInitiateConnection();
        }
    }
}

void IConnection::onInitiateConnectionSuccess(CommunicationPractices practices) {
    m_connected = true;
    m_practices = practices;
    m_processTreeStaleTimer.start();
    m_systemInformationStaleTimer.start();
    m_processFilterResultStaleTimer.start();
    // Also start stuff immediately!
    emit requestProcessTree();
    emit requestSystemInformation();
}

void IConnection::onInitiateConnectionError(ConnectionError error) {
    m_connected = false;
    m_processTreeStaleTimer.stop();
    m_systemInformationStaleTimer.stop();
    m_processFilterResultStaleTimer.stop();
}

#define MUST_HAVE_CONNECTED                    \
    {                                          \
        if (!m_connected)                      \
            return;                            \
    }

void IConnection::requestProcessTree() {
    MUST_HAVE_CONNECTED
    doRequestProcessTree(m_lastProcessTreeTimestamp);
}

void IConnection::requestSystemInformation() {
    MUST_HAVE_CONNECTED
    doRequestSystemInformation();
}

void IConnection::requestProcessFiltering(
    QSet<proc_id_t> pidSet,
    QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
) {
    MUST_HAVE_CONNECTED

    {
        m_filtering = true;
        m_processFilterResultStaleTimer.stop();
        doRequestProcessFiltering(pidSet, filterArgsList);
    }
}

void IConnection::requestProcessAction(process_action_id_t actionId, proc_id_t pid) {
    MUST_HAVE_CONNECTED
    doRequestProcessAction(actionId, pid);
}

void IConnection::onReplyProcessFiltering(
    QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
    QSet<proc_id_t> filteredPidSet
) {
    MUST_HAVE_CONNECTED

    {
        if (m_filtering) {
            m_processFilterResultStaleTimer.setInterval(
                m_practices.refilterIntervalInSeconds
            );
            m_processFilterResultStaleTimer.start();
        }
    }
}

void IConnection::cancelProcessFilteringInterval() {
    // MUST_HAVE_CONNECTED
    {
        m_filtering = false;
        m_processFilterResultStaleTimer.stop();
    }
}

void IConnection::cacheLastProcessTreeTimestampAsNeeded(ThreadsafeSharedConstPointer<ProcessTree> newTree) {
    if (newTree.get() == nullptr) {
        return;
    }

    {
        m_lastProcessTreeTimestamp = newTree.get()->timestamp;
    }
}

#undef MUST_HAVE_CONNECTED
