#ifndef BaseProcessItemModel_INCLUDED
#define BaseProcessItemModel_INCLUDED

#include "taskman/common/types.h"
#include "taskman/connections/IConnection.h"
#include <QAbstractItemModel>
#include <QHash>
#include <QObject>

static_assert(sizeof(quintptr) >= sizeof(uint64_t), "By now, quintptr must be at least 64 bits. Consider building this program on a 64-bit system (or more!). In the future this may be reworked to support 32-bit as well - it is not impossible technically.");

class BaseProcessItemModel : public QAbstractItemModel {
    Q_OBJECT

public:
    /**
     * Instances of this class do not take ownership of the connection handle.
     */
    BaseProcessItemModel(IConnection* connection, QObject* parent = nullptr);
    virtual ~BaseProcessItemModel() override;

    proc_id_t indexToPid(QModelIndex const& index) const;
    static proc_id_t anyIndexToPid(QModelIndex const& index, proc_id_t imaginaryRootProcId);
    QModelIndex pidToIndex(proc_id_t pid) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual int columnCount(QModelIndex const& = {}) const override;
    virtual QModelIndex index(int row, int column, QModelIndex const& parentIndex = {}) const override;
    virtual QModelIndex parent(QModelIndex const& childIndex) const override;
    virtual int rowCount(QModelIndex const& parentIndex = {}) const override;
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;

    QList<ProcessField> const& getProcessFields() const;
    IConnection* getConnection() const;
    QHash<proc_id_t, ProcessData> snapshot() const;

public slots:
    void initialize();
    void onReplyProcessFiltering(
        QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
        QSet<proc_id_t> filteredPidSet
    );

protected:
    /**
     * A subclass may reimplement this function for more colorful
     * display of data suited for the platform!
     */
    virtual QVariant customData(QModelIndex const& index, int role, proc_id_t pid, ProcessData const& processData) const;

private slots:
    void rebuildTreeOnUI(ThreadsafeSharedConstPointer<ProcessTree> newTree);

private:
    IConnection* const m_connection;
    QList<ProcessField> m_fields;
    QHash<proc_id_t, ProcessData> m_pidToProcessDataMap;
    proc_count_t m_numProcs;

    proc_id_t getImaginaryRootProcId() const;
    proc_id_t m__imaginaryRootProcId;

    QSet<proc_id_t> m_filteredPidSet;
    bool m_filtering;

    /**
     * Translate/migrate content of rootBefore to be like that of rootAfter
     */
    void diffAndMigrate(
        QHash<proc_id_t, ProcessData>& mapBefore,
        ProcessData& rootBefore,
        QHash<proc_id_t, ProcessData> const& mapAfter,
        ProcessData const& rootAfter
    );

    /**
     * Remove recursively (DFS)
     */
    void removeAllChildrenOf(QHash<proc_id_t, ProcessData>& map, ProcessData& root);

    /**
     * Add (BFS)
     */
    void addAllAsChildrenOf(
        QHash<proc_id_t, ProcessData>& mapBefore,
        ProcessData& root,
        QHash<proc_id_t, ProcessData> const& mapAfter,
        QSet<proc_id_t> const& newChildProcIds
    );
};

#endif // BaseProcessItemModel_INCLUDED
