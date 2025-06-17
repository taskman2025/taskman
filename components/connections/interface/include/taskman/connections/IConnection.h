#ifndef IConnection_INCLUDED
#define IConnection_INCLUDED

#include "taskman/backends/IBackend.h"
#include "taskman/backends/ProcessTree.h"
#include "taskman/backends/SystemInformation.h"
#include "taskman/common/ThreadsafeSharedConstPointer.h"
#include <QFuture>
#include <QList>
#include <QString>
#include <QTimer>
#include <QVariant>

enum class ConnectionErrorCode {
    NONE,
    OTHER,
    CONNECTION_INITIALIZATION_FAILURE,
    CONNECTION_INITIATION_FAILURE,
    BACKEND_FAILURE
};

struct ConnectionError {
    ConnectionErrorCode errorCode;
    ResponseErrorCode resErrorCode;
    QString errorMessage;

    QString manifest() {
        return QString("Code: %1\nBackend code: %2\nMessage: %3")
            .arg(static_cast<int>(errorCode))
            .arg(static_cast<int>(resErrorCode))
            .arg(errorMessage);
    }
};

/**
 * Network-oriented design: (almost) everything public
 * is either signal or slot!
 *
 * An instance of this class should be pushed
 * into a worker thread for parallel processing.
 * i.e. connection->moveToThread();
 */
class IConnection : public QObject {
    Q_OBJECT

public:
    IConnection();
    virtual ~IConnection() = 0;
    bool isConnected() const;
    virtual IPlatformProfile const& getPlatformProfile() const = 0;

    ////////////////////////////////////
public slots:
    void initiateConnection();
signals:
    void initialized();
signals:
    void initializationError(ConnectionError e);
signals:
    void connectionInitiated(CommunicationPractices practices);
signals:
    void connectionInitiationError(ConnectionError e);
    ////////////////////////////////////

    ////////////////////////////////////
signals:
    void replyProcessTree(ThreadsafeSharedConstPointer<ProcessTree> newTree);
signals:
    void replyProcessTreeError(ConnectionError e);
    ////////////////////////////////////

    ////////////////////////////////////
signals:
    void replySystemInformation(SystemInformation s);
signals:
    void replySystemInformationError(ConnectionError e);
    ////////////////////////////////////

    ////////////////////////////////////
public slots:
    void requestProcessAction(process_action_id_t actionId, proc_id_t pid);
signals:
    void replyProcessAction(ActionResult result);
signals:
    void replyProcessActionError(ConnectionError e);
    ////////////////////////////////////

    ////////////////////////////////////
public slots:
    void requestProcessFiltering(
        QSet<proc_id_t> pidSet,
        QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
    );
    void cancelProcessFilteringInterval();
signals:
    void replyProcessFiltering(
        QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
        QSet<proc_id_t> filteredPidSet
    );
signals:
    void replyProcessFilteringError(ConnectionError e);
signals:
    void refilterProcessesNow();
    ////////////////////////////////////

protected:
    /**
     * An implementation must signal initialized() upon success,
     * initializationError() on failure.
     */
    virtual void doInitialize() = 0;

    /**
     * An implementation must signal connectionInitiated() upon success,
     * connectionInitiationError() on failure.
     */
    virtual void doInitiateConnection() = 0;

    /**
     * An implementation must signal replyProcessTree() upon success,
     * replyProcessTreeError() on failure.
     */
    virtual void doRequestProcessTree(timestamp_t lastTreeTimestamp) = 0;

    /**
     * An implementation must signal replySystemInformation() upon success,
     * replySystemInformationError() on failure.
     */
    virtual void doRequestSystemInformation() = 0;

    /**
     * An implementation must signal replyProcessAction upon success,
     * replyProcessActionError() on failure.
     */
    virtual void doRequestProcessAction(process_action_id_t actionId, proc_id_t pid) = 0;

    /**
     * An implementation must signal replyProcessFiltering() upon success,
     * replyProcessFilteringError() on failure.
     */
    virtual void doRequestProcessFiltering(
        QSet<proc_id_t> pidSet,
        QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
    ) = 0;

private slots:
    void onInitiateConnectionSuccess(CommunicationPractices practices);
    void onInitiateConnectionError(ConnectionError error);
    void onReplyProcessFiltering(
        QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
        QSet<proc_id_t> filteredPidSet
    );
    void requestProcessTree();
    void requestSystemInformation();
    void cacheLastProcessTreeTimestampAsNeeded(ThreadsafeSharedConstPointer<ProcessTree> newTree);

private:
    CommunicationPractices m_practices;

    QTimer m_systemInformationStaleTimer;
    QTimer m_processTreeStaleTimer;
    QTimer m_processFilterResultStaleTimer;

    timestamp_t m_lastProcessTreeTimestamp;

    bool m_initialized;
    bool m_connected;
    bool m_filtering;
};

#endif // IConnection_INCLUDED
