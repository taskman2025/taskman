#include "taskman/frontends/BaseProcessItemModel.h"
#include <QColor>

BaseProcessItemModel::BaseProcessItemModel(
    IConnection* connection,
    QObject* parent
)
    : m_connection{connection},
      m_fields{connection->getPlatformProfile().getProcessFields()}, // TODO: selective fields
      m_pidToProcessDataMap{},
      m_numProcs{0},
      m__imaginaryRootProcId{m_connection->getPlatformProfile().getImaginaryRootProcId()},
      m_filteredPidSet{},
      m_filtering{false},
      QAbstractItemModel(parent) {
}

BaseProcessItemModel::~BaseProcessItemModel() = default;

proc_id_t BaseProcessItemModel::getImaginaryRootProcId() const {
    return m__imaginaryRootProcId;
}

proc_id_t BaseProcessItemModel::anyIndexToPid(
    QModelIndex const& index,
    proc_id_t imaginaryRootPid
) {
    if (!index.isValid()) {
        return imaginaryRootPid;
    }
    return static_cast<proc_id_t>(index.internalId());
}

proc_id_t BaseProcessItemModel::indexToPid(QModelIndex const& index) const {
    return anyIndexToPid(index, getImaginaryRootProcId());
}

QModelIndex BaseProcessItemModel::pidToIndex(proc_id_t pid) const {
    if (pid == getImaginaryRootProcId()) {
        return QModelIndex{};
    }
    if (!m_pidToProcessDataMap.contains(pid)) {
        return QModelIndex{};
    }
    proc_id_t ppid = m_pidToProcessDataMap[pid].getPPID();
    if (!m_pidToProcessDataMap.contains(ppid)) {
        return QModelIndex{};
    }
    QList<proc_id_t> const& siblings = m_pidToProcessDataMap[ppid].getChildrenProcIds();
    int row = siblings.indexOf(pid);
    if (row < 0)
        return QModelIndex{};
    return createIndex(row, 0, static_cast<quintptr>(pid));
}

QVariant BaseProcessItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section >= 0 && section < m_fields.size()) {
                return m_fields[section].name;
            }
        } else if (role == Qt::ToolTipRole) {
            if (section >= 0 && section < m_fields.size()) {
                return m_fields[section].description;
            }
        }
    }
    return {};
}

int BaseProcessItemModel::columnCount(QModelIndex const&) const {
    return m_fields.size();
}

QModelIndex BaseProcessItemModel::index(int row, int column, QModelIndex const& parentIndex) const {
    proc_id_t parentPid = indexToPid(parentIndex);
    if (!m_pidToProcessDataMap.contains(parentPid)) {
        return QModelIndex{};
    }
    ProcessData const& parentNode = m_pidToProcessDataMap[parentPid];

    QList<proc_id_t> const& childrenProcIds = parentNode.getChildrenProcIds();
    if (row < 0 || row >= childrenProcIds.size()) {
        return QModelIndex{};
    }
    if (column < 0 || column >= m_fields.size()) {
        return QModelIndex{};
    }
    proc_id_t pid = childrenProcIds[row];
    return createIndex(row, column, static_cast<quintptr>(pid));
}

QModelIndex BaseProcessItemModel::parent(QModelIndex const& childIndex) const {
    if (!childIndex.isValid())
        return QModelIndex{};

    proc_id_t childPid = indexToPid(childIndex);
    if (!m_pidToProcessDataMap.contains(childPid)) {
        return QModelIndex{};
    }

    ProcessData const& childNode = m_pidToProcessDataMap[childPid];
    proc_id_t parentPid = childNode.getPPID();

    return pidToIndex(parentPid);
}

int BaseProcessItemModel::rowCount(QModelIndex const& index) const {
    proc_id_t pid = indexToPid(index);
    if (!m_pidToProcessDataMap.contains(pid)) {
        return 0;
    }
    ProcessData const& node = m_pidToProcessDataMap[pid];
    return node.getChildrenProcIds().size();
}

QVariant BaseProcessItemModel::data(QModelIndex const& index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    proc_id_t pid = indexToPid(index);
    if (!m_pidToProcessDataMap.contains(pid)) {
        return {};
    }
    ProcessData const& processData = m_pidToProcessDataMap[pid];

    int columnNumber = index.column();

    if (role == Qt::DisplayRole) {
        if (columnNumber < 0 || columnNumber >= m_fields.size()) {
            return {};
        }
        field_mask_t fieldBit = m_fields[columnNumber].mask;
        return processData.getFieldValue(fieldBit);
    } else if (role == Qt::ToolTipRole) {
        if (columnNumber < 0 || columnNumber >= m_fields.size()) {
            return {};
        }
        return m_fields[columnNumber].description;
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(pid);
    } else if (role == Qt::BackgroundRole && m_filtering) {
        if (m_filteredPidSet.contains(pid)) {
            return QColor(255, 192, 203);
        }
    }
    // TODO: Pink background for filtered rows
    return customData(index, role, pid, processData);
}

QVariant BaseProcessItemModel::customData(QModelIndex const& index, int role, proc_id_t pid, ProcessData const& processData) const {
    return {};
}

// void BaseProcessItemModel::setFields(QList<ProcessField> const& fields) {
//     m_fields = fields;
//     emit headerDataChanged(Qt::Horizontal, 0, m_fields.size() - 1);
// }

void BaseProcessItemModel::removeAllChildrenOf(QHash<proc_id_t, ProcessData>& map, ProcessData& root) {
    int const N = root.getChildrenProcIds().size();
    if (N == 0) {
        return;
    }

    for (proc_id_t childPid : root.getChildrenProcIds()) {
        ProcessData& childNode = map[childPid];
        if (!childNode.getChildrenProcIds().isEmpty()) {
            removeAllChildrenOf(map, childNode);
        }
    }

    beginRemoveRows(pidToIndex(root.getPID()), 0, N - 1);
    /**
     * Exhaustive removal of unused PIDs in the map
     * is not recommended since they might get reused
     * at the very next iteration!
     */
#ifdef EXHAUSTIVE_REMOVAL
    for (proc_id_t childPid : root.getChildrenProcIds()) {
        map.remove(childPid);
    }
#endif // EXHAUSTIVE_REMOVAL
    root.clearChildren();
    endRemoveRows();
}

void BaseProcessItemModel::addAllAsChildrenOf(
    QHash<proc_id_t, ProcessData>& mapBefore,
    ProcessData& root,
    QHash<proc_id_t, ProcessData> const& mapAfter,
    QSet<proc_id_t> const& newChildProcIds
) {
    int const N = newChildProcIds.size();
    if (N == 0) {
        return;
    }

    int const base = root.getChildrenProcIds().size();
    if (N > 0) {
        beginInsertRows(pidToIndex(root.getPID()), base, base + N - 1);
        for (proc_id_t pid : newChildProcIds) {
            mapBefore.insert(pid, mapAfter[pid]);
            mapBefore[pid].clearChildren();
            root.addChildProcId(pid);
        }
        endInsertRows();

        for (proc_id_t pid : newChildProcIds) {
            addAllAsChildrenOf(mapBefore, mapBefore[pid], mapAfter, mapAfter[pid].getChildrenProcIdSet());
        }
    }
}

void BaseProcessItemModel::diffAndMigrate(
    QHash<proc_id_t, ProcessData>& mapBefore,
    ProcessData& rootBefore,
    QHash<proc_id_t, ProcessData> const& mapAfter,
    ProcessData const& rootAfter
) {
    QSet<proc_id_t> const& childrenRootBefore = rootBefore.getChildrenProcIdSet();
    QSet<proc_id_t> const& childrenRootAfter = rootAfter.getChildrenProcIdSet();

    QSet<proc_id_t> pidsToAdd = childrenRootAfter - childrenRootBefore;
    QSet<proc_id_t> pidsToRemove = childrenRootBefore - childrenRootAfter;
    QSet<proc_id_t> pidsToModify{childrenRootBefore};
    pidsToModify.intersect(childrenRootAfter);

    for (auto it = pidsToModify.begin(); it != pidsToModify.end();) {
        proc_id_t pid = *it;
        // Edge case:
        // If ppid changes then we actually need to remove and add it again!
        ProcessData& rowBefore = mapBefore[pid];
        ProcessData const& rowAfter = mapAfter[pid];
        if (rowAfter.getPPID() != rowBefore.getPPID()) {
            it = pidsToModify.erase(it);
            pidsToRemove.insert(pid);
            pidsToAdd.insert(pid);
        } else {
            ++it;
        }
    }

    // 1. REMOVE
    for (proc_id_t pid : pidsToRemove) {
        ProcessData& rowToRemove = mapBefore[pid];
        removeAllChildrenOf(mapBefore, rowToRemove);
        int positionOfPidToRemove = rootBefore.getChildrenProcIds().indexOf(pid);
        beginRemoveRows(pidToIndex(rootBefore.getPID()), positionOfPidToRemove, positionOfPidToRemove);
        rootBefore.removeChildProcId(pid);
#ifdef EXHAUSTIVE_REMOVAL
        mapBefore.remove(pid);
#endif // EXHAUSTIVE_REMOVAL
        endRemoveRows();
    }

    // 2. ADD
    addAllAsChildrenOf(mapBefore, rootBefore, mapAfter, pidsToAdd);

    // 3. MODIFY
    for (proc_id_t pid : pidsToModify) {
        if (!mapBefore.contains(pid) || !mapAfter.contains(pid)) {
            throw std::runtime_error{QString("modified pid (%1) appear neither in mapBefore nor mapAfter - this should never happen").arg(pid).toStdString()};
        }
        diffAndMigrate(mapBefore, mapBefore[pid], mapAfter, mapAfter[pid]);
    }
}

void BaseProcessItemModel::rebuildTreeOnUI(ThreadsafeSharedConstPointer<ProcessTree> newTree) {
    if (newTree.get() == nullptr) {
        qDebug() << "tree not changed";
        return;
    }

    proc_id_t rootId = getImaginaryRootProcId();
    if (!m_pidToProcessDataMap.contains(rootId)) {
        // Rebuild tree entirely, since even the root is not there for us to diff!
        beginResetModel();
        m_pidToProcessDataMap.clear();
        m_pidToProcessDataMap = newTree->pidToProcessDataMap;
        m_pidToProcessDataMap.detach();
        endResetModel();
    } else {
        diffAndMigrate(
            m_pidToProcessDataMap,
            m_pidToProcessDataMap[rootId],
            newTree->pidToProcessDataMap,
            newTree->pidToProcessDataMap[rootId]
        );
    }
}

void BaseProcessItemModel::initialize() {
    connect(
        m_connection,
        &IConnection::replyProcessTree,
        this,
        &BaseProcessItemModel::rebuildTreeOnUI
    );
}

QList<ProcessField> const& BaseProcessItemModel::getProcessFields() const {
    return m_fields;
}

IConnection* BaseProcessItemModel::getConnection() const {
    return m_connection;
}

QHash<proc_id_t, ProcessData> BaseProcessItemModel::snapshot() const {
    QHash<proc_id_t, ProcessData> map{m_pidToProcessDataMap};
    map.detach();
    return map;
}

void BaseProcessItemModel::onReplyProcessFiltering(
    QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
    QSet<proc_id_t> filteredPidSet
) {
    m_filtering = true;
    m_filteredPidSet = filteredPidSet;
    m_filteredPidSet.detach();

    for (proc_id_t pid : m_filteredPidSet) {
        QModelIndex index = pidToIndex(pid);
        if (index.isValid()) {
            emit dataChanged(index, index, {Qt::BackgroundRole});
        }
    }
}
