#include "taskman/backend/IProcessItemModel.h"
#include <QColor>
#include <QMutexLocker>
#include <QSet>

IProcessItemModel::IProcessItemModel(IPlatform& platform, QObject* parent) : m_platform{platform}, QAbstractItemModel(parent) {
    setFields(platform.getProcessFields());

    connect(
        this, &IProcessItemModel::collectTreeComplete,
        this, &IProcessItemModel::rebuildTreeOnUI,
        Qt::QueuedConnection
    );
}

IProcessItemModel::~IProcessItemModel() {}

proc_id_t IProcessItemModel::indexToPid(QModelIndex const& index) const {
    if (!index.isValid()) {
        return getImaginaryRootProcId();
    }
    return static_cast<proc_id_t>(index.internalId());
}

QModelIndex IProcessItemModel::pidToIndex(proc_id_t pid) const {
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

QVariant IProcessItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

int IProcessItemModel::columnCount(QModelIndex const&) const {
    return m_fields.size();
}

QModelIndex IProcessItemModel::index(int row, int column, QModelIndex const& parentIndex) const {
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

QModelIndex IProcessItemModel::parent(QModelIndex const& childIndex) const {
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

int IProcessItemModel::rowCount(QModelIndex const& index) const {
    proc_id_t pid = indexToPid(index);
    if (!m_pidToProcessDataMap.contains(pid)) {
        return 0;
    }
    ProcessData const& node = m_pidToProcessDataMap[pid];
    return node.getChildrenProcIds().size();
}

QVariant IProcessItemModel::data(QModelIndex const& index, int role) const {
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
        if (columnNumber < 0) {
            return QVariant::fromValue(processData);
        }
        if (columnNumber >= m_fields.size()) {
            return {};
        }
        field_mask_t fieldBit = m_fields[columnNumber].mask;
        if (fieldBit == FIELD_FILTERED) {
            return processData.getFieldValue(FIELD_FILTERED).toBool() ? "x" : "";
        }

        return processData.getFieldValue(fieldBit);
    } else if (role == Qt::ToolTipRole) {
        if (columnNumber < 0 || columnNumber >= m_fields.size()) {
            return {};
        }
        return m_fields[columnNumber].description;
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(pid);
    }
    return customData(index, role, pid, processData);
}

QVariant IProcessItemModel::customData(QModelIndex const& index, int role, proc_id_t pid, ProcessData const& processData) const {
    if (role == Qt::BackgroundRole) {
        int columnNumber = index.column();

        if (columnNumber < 0 || columnNumber >= m_fields.size()) {
            return {};
        }
        if (m_fields[columnNumber].mask == FIELD_FILTERED) {
            return processData.getFieldValue(FIELD_FILTERED).toBool() ? QVariant{QColor("#FFB6C1")} : QVariant{};
        }
    }
    return {};
}

void IProcessItemModel::setFields(QList<ProcessField> const& fields) {
    m_fields = fields;
    emit headerDataChanged(Qt::Horizontal, 0, m_fields.size() - 1);
}

void IProcessItemModel::removeAllChildrenOf(QHash<proc_id_t, ProcessData>& map, ProcessData& root) {
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
    root.clearChildren();
    endRemoveRows();
}

void IProcessItemModel::addAllAsChildrenOf(
    QHash<proc_id_t, ProcessData>& mapBefore,
    ProcessData& root,
    QHash<proc_id_t, ProcessData>& mapAfter,
    QSet<proc_id_t> const& newChildProcIds
) {
    int const N = newChildProcIds.size();
    if (N == 0) {
        return;
    }
    int const base = root.getChildrenProcIds().size();

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

void IProcessItemModel::diffAndMigrate(QHash<proc_id_t, ProcessData>& mapBefore, ProcessData& rootBefore, QHash<proc_id_t, ProcessData>& mapAfter, ProcessData const& rootAfter) {
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
        endRemoveRows();
    }

    // 2. ADD
    addAllAsChildrenOf(mapBefore, rootBefore, mapAfter, pidsToAdd);

    // 3. MODIFY
    for (proc_id_t pid : pidsToModify) {
        diffAndMigrate(mapBefore, mapBefore[pid], mapAfter, mapAfter[pid]);
    }
}

void IProcessItemModel::rebuildTreeOnUI(QHash<proc_id_t, ProcessData> newMap, proc_count_t numProcs) {
    QMutexLocker lock{&m_pidToProcessDataMapWriteMutex};

    m_numProcs = numProcs;

    proc_id_t const imaginaryRootPid = getImaginaryRootProcId();

    if (!m_pidToProcessDataMap.contains(imaginaryRootPid)) {
        // Rebuild entirely, since even the root is not there!
        beginResetModel();
        m_pidToProcessDataMap.clear();
        m_pidToProcessDataMap = newMap;
        endResetModel();
    } else {
        diffAndMigrate(m_pidToProcessDataMap, m_pidToProcessDataMap[imaginaryRootPid], newMap, newMap[imaginaryRootPid]);
    }
    emit rebuildTreeComplete(numProcs);
}

void IProcessItemModel::collectTree() {
    proc_id_t const imaginaryRootPid = getImaginaryRootProcId();

    field_mask_t fields = 0;
    for (auto const& field : m_fields) {
        fields |= field.mask;
    }

    QSharedPointer<IProcessReader> processReader{m_platform.startReadingProcesses({.fields = fields, .filters = m_filters})};

    std::function<bool(ProcessData&)> getNext = [processReader, fields, this](ProcessData& processData) -> bool {
        if (processReader->next()) {
            processData.setPID(processReader->getCurrentProcId());
            processData.setPPID(processReader->getCurrentProcPPID());
            for (auto const& field : m_fields) {
                if (hasField(fields, field.mask)) {
                    processData.setFieldValue(field.mask, processReader->getCurrentProcData(field.mask));
                }
            }
            return true;
        }
        return false;
    };

    QHash<proc_id_t, ProcessData> newMap;

    proc_count_t N = 0;
    ProcessData proc;

    while (getNext(proc)) {
        proc_id_t pid = proc.getPID();
        proc_id_t ppid = proc.getPPID();

        bool ok;
        validateIds(pid, ppid, ok);
        if (!ok) {
            continue;
        }

        // Defensive sanity check (skip pathological entry)
        if (pid != imaginaryRootPid && pid == ppid) {
            qWarning() << "process " << pid << " has itself as parent, skipping";
            continue;
        }

        ++N;

        proc.setPID(pid);
        proc.setPPID(ppid);

        if (!newMap.contains(pid)) {
            newMap.insert(pid, proc);
        } else {
            newMap[pid].update(proc);
            qInfo() << pid << newMap[pid].getPID() << newMap[pid].getChildrenProcIds() << proc.getChildrenProcIds();
        }

        if (!newMap.contains(ppid)) {
            ProcessData dummyParent;
            dummyParent.setPID(ppid);
            dummyParent.setPPID(imaginaryRootPid);
            dummyParent.addChildProcId(pid);
            newMap.insert(ppid, dummyParent);
        } else {
            newMap[ppid].addChildProcId(pid);
        }
    }

    emit collectTreeComplete(newMap, N);
}

void IProcessItemModel::startRebuildingTree() {
    auto future = QtConcurrent::run([this]() {
        this->collectTree();
    });
}

void IProcessItemModel::addFilter(ThreadsafeConstSharedPointer<IProcessFilter> filter) {
    if (!filter->isApplicable())
        return;
    for (ThreadsafeConstSharedPointer<IProcessFilter> activeFilter : m_filters) {
        if (*activeFilter == *filter) {
            return;
        }
    }
    m_filters.append(filter);
}

void IProcessItemModel::clearFilter() {
    m_filters.clear();
}
