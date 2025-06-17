#ifndef ILocalConnection_INCLUDED
#define ILocalConnection_INCLUDED

#include "taskman/backends/ILocalBackend.h"
#include "taskman/connections/IConnection.h"

class ILocalConnection : public IConnection {
    Q_OBJECT

public:
    ILocalConnection(ILocalBackend& backend);
    virtual IPlatformProfile const& getPlatformProfile() const override;
    virtual ~ILocalConnection() = 0;

    ILocalBackend& m_backend;

    backend_request_id_t const m_id;

private:
    static backend_request_id_t getNextId();

protected:
    /**
     * An implementation must signal initialized() upon success,
     * initializationError() on failure.
     */
    virtual void doInitialize() override;

    /**
     * An implementation must signal connectionInitiated() upon success,
     * connectionInitiationError() on failure.
     */
    virtual void doInitiateConnection() override;

    /**
     * An implementation must signal replyProcessTree() upon success,
     * replyProcessTreeError() on failure.
     */
    virtual void doRequestProcessTree(timestamp_t lastTreeTimestamp) override;

    /**
     * An implementation must signal replyProcessAction upon success,
     * replyProcessActionError() on failure.
     */
    virtual void doRequestProcessAction(process_action_id_t actionId, proc_id_t pid) override;

    /**
     * An implementation must signal replySystemInformation() upon success,
     * replySystemInformationError() on failure.
     */
    virtual void doRequestSystemInformation() override;

    /**
     * An implementation must signal replyProcessFiltering() upon success,
     * replyProcessFilteringError() on failure.
     */
    virtual void doRequestProcessFiltering(
        QSet<proc_id_t> pidSet,
        QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
    ) override;

signals:
    void i_requestCommunicationPractices(backend_request_id_t id);
private slots:
    void i_replyCommunicationPractices(
        backend_request_id_t id,
        ResponseError error,
        CommunicationPractices practices
    );

signals:
    void i_requestSystemInformation(backend_request_id_t id);
private slots:
    void i_replySystemInformation(
        backend_request_id_t id,
        ResponseError error, SystemInformation s
    );

signals:
    void i_requestProcessAction(
        backend_request_id_t id,
        process_action_id_t actionId,
        proc_id_t pid
    );
private slots:
    void i_replyProcessAction(
        backend_request_id_t id,
        ResponseError error,
        ActionResult result
    );

signals:
    void i_requestProcessTree(backend_request_id_t id, timestamp_t lastTimeFetched);
private slots:
    void i_replyProcessTree(
        backend_request_id_t id,
        ResponseError error,
        ThreadsafeSharedConstPointer<ProcessTree> tree
    );

signals:
    void i_requestProcessFiltering(
        backend_request_id_t id,
        QSet<proc_id_t> pidSet,
        QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
    );
private slots:
    void i_replyProcessFiltering(
        backend_request_id_t id,
        ResponseError error,
        QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
        QSet<proc_id_t> filteredPidSet
    );
};

#endif // ILocalConnection_INCLUDED
