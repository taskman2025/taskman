#ifndef IPROCESSITEMMODEL_INCLUDED
#define IPROCESSITEMMODEL_INCLUDED

#include "taskman/backend/ProcessData.h"
#include "taskman/common/types.h"
#include "taskman/common/ThreadsafeSharedConstPointer.h"
#include "taskman/platform/IPlatform.h"
#include <QHash>
#include <QObject>
#include <QtConcurrent>
#include <functional>
#include <QMutex>

static_assert(sizeof(quintptr) >= sizeof(uint64_t), "quintptr must be at least 64 bits. Consider building this program on a 64-bit system (or more!).");

class IProcessItemModel : public QAbstractItemModel {
    Q_OBJECT
public:
    IProcessItemModel(IPlatform& platform, QObject* parent = nullptr);
    virtual ~IProcessItemModel() override;
    /**
     * Returns the parent of all processes, even init(pid=1) on Linux.
     * Choose a plausible value for the platform - e.g. -1 on Linux.
     * In that case, GenericProcessRawData struct object representing the "init" process
     * must have .pid = 1 and .ppid = -1.
     */
    virtual proc_id_t getImaginaryRootProcId() const = 0;

    /**
     * Validates and normalizes the IDs. If one of them is corrupt,
     * set ok = false.
     */
    virtual void validateIds(proc_id_t& pid, proc_id_t& ppid, bool& ok) const = 0;

    void getNumProcs() const;

    proc_id_t indexToPid(QModelIndex const& index) const;
    QModelIndex pidToIndex(proc_id_t pid) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual int columnCount(QModelIndex const& = {}) const override;
    virtual QModelIndex index(int row, int column, QModelIndex const& parentIndex = {}) const override;
    virtual QModelIndex parent(QModelIndex const& childIndex) const override;
    virtual int rowCount(QModelIndex const& parentIndex = {}) const override;
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
    void setFields(QList<ProcessField> const& fields);
    void addFilter(ThreadsafeConstSharedPointer<IProcessFilter> filter);
    // void removeFilterByType(IProcessFilter* filter); // TODO
    void clearFilter();

    // virtual QString getStringForFiltering(ProcessData const& data) const = 0;

    virtual QVariant customData(QModelIndex const& index, int role, proc_id_t pid, ProcessData const& processData) const;

public slots:
    void startRebuildingTree();
    void rebuildTreeOnUI(QHash<proc_id_t, ProcessData> newMap, proc_count_t numProcs);

signals:
    void collectTreeComplete(QHash<proc_id_t, ProcessData> newMap, proc_count_t numProcs);
    void rebuildTreeComplete(proc_count_t numProcs);

protected:
    QList<ProcessField> m_fields;
    QList<ThreadsafeConstSharedPointer<IProcessFilter>> m_filters;
    QHash<proc_id_t, ProcessData> m_pidToProcessDataMap;
    proc_count_t m_numProcs;
    QMutex m_pidToProcessDataMapWriteMutex;
    IPlatform& m_platform;

private:
    void collectTree();

    /**
     * Translate/migrate content of rootBefore to be like that of rootAfter
     */
    void diffAndMigrate(QHash<proc_id_t, ProcessData>& mapBefore, ProcessData& rootBefore, QHash<proc_id_t, ProcessData>& mapAfter, ProcessData const& rootAfter);

    /**
     * Remove recursively (DFS)
     */
    void removeAllChildrenOf(QHash<proc_id_t, ProcessData>& map, ProcessData& root);

    /**
     * Add (BFS)
     */
    void addAllAsChildrenOf(QHash<proc_id_t, ProcessData>& mapBefore, ProcessData& root, QHash<proc_id_t, ProcessData>& mapAfter, QSet<proc_id_t> const& newChildProcIds);
};

#endif // IPROCESSITEMMODEL_INCLUDED
