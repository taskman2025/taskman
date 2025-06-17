#ifndef IBackend_INCLUDED
#define IBackend_INCLUDED

#include "taskman/backends/ProcessTree.h"
#include "taskman/backends/SystemInformation.h"
#include "taskman/common/ThreadsafeConstReadProxy.h"
#include "taskman/common/types.h"
#include "taskman/platform_runtimes/IPlatformRuntime.h"
#include <QHash>
#include <QObject>
#include <QtConcurrent>

enum class ResponseErrorCode {
    NONE,
    OTHER,
};

struct ResponseError {
    ResponseErrorCode errorCode;
    QString errorMessage;
};

struct CommunicationPractices {
    qint64 processUpdateIntervalInSeconds;
    qint64 systemInformationUpdateIntervalInSeconds;
    qint64 refilterIntervalInSeconds;
};

/**
 * Network-oriented design: almost everything public
 * is either signal or slot!
 *
 * An instance of this class should be pushed
 * into a worker thread for parallel processing.
 * i.e. backend->moveToThread();
 */
class IBackend : public QObject {
    Q_OBJECT

public:
    IBackend(IPlatformRuntime& platformRuntime);
    IPlatformRuntime& getPlatformRuntime() const;
    virtual ~IBackend() = 0;

public slots:
    void requestSystemInformation(backend_request_id_t id);
signals:
    void replySystemInformation(
        backend_request_id_t id,
        ResponseError error, SystemInformation s
    );

public slots:
    void requestProcessTree(backend_request_id_t id, timestamp_t lastTimeFetched);
    /**
     * tree == nullptr means NOT MODIFIED SINCE lastTimeFetched
     */
signals:
    void replyProcessTree(
        backend_request_id_t id,
        ResponseError error,
        ThreadsafeSharedConstPointer<ProcessTree> tree
    );

public slots:
    void requestCommunicationPractices(backend_request_id_t id);
signals:
    void replyCommunicationPractices(
        backend_request_id_t id,
        ResponseError error,
        CommunicationPractices practices
    );

public slots:
    void requestProcessFiltering(
        backend_request_id_t id,
        QSet<proc_id_t> pidSet,
        QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
    );
signals:
    void replyProcessFiltering(
        backend_request_id_t id,
        ResponseError error,
        QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
        QSet<proc_id_t> filteredPidSet
    );

public slots:
    void requestProcessAction(
        backend_request_id_t id,
        process_action_id_t actionId,
        proc_id_t pid
    );
signals:
    void replyProcessAction(
        backend_request_id_t id,
        ResponseError error,
        ActionResult result
    );

protected:
    virtual qint64 getProcessTreeUpdateIntervalInSeconds() const = 0;
    virtual qint64 getSystemInformationUpdateIntervalInSeconds() const = 0;
    virtual qint64 getRefilterIntervalInSeconds() const = 0;

private:
    ThreadsafeConstReadProxy<ProcessTree> m_savedProcessTree;
    IPlatformRuntime& m_platformRuntime;
    QFuture<void> m_updateTreeFuture;

    void doReplyProcessTree(
        backend_request_id_t id,
        ResponseError error,
        ThreadsafeSharedConstPointer<ProcessTree> tree
    );
    void doReplySystemInformation(
        backend_request_id_t id,
        ResponseError error,
        SystemInformation s
    );
    void doReplyProcessAction(
        backend_request_id_t id,
        ResponseError error,
        ActionResult result
    );
    QFuture<void> buildTree();
    static QFuture<ProcessTree> doBuildProcessTree(IPlatformRuntime& platformRuntime);
};

#endif // IBackend_INCLUDED
