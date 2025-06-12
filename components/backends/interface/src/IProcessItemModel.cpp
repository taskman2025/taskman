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
    if (childPid == getImaginaryRootProcId() || !m_pidToProcessDataMap.contains(childPid)) {
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
    if (root.getChildrenProcIds().isEmpty())
        return;

    bool allChildrenAreNowLeaf = true;
    int N = 0;
    for (;;) {
        N = root.getChildrenProcIds().size();
        for (proc_id_t childPid : root.getChildrenProcIds()) {
            if (!map.contains(childPid)) {
                N -= 1;
                continue;
            }
            ProcessData& childNode = map[childPid];
            if (!childNode.getChildrenProcIds().isEmpty()) {
                allChildrenAreNowLeaf = false;
                removeAllChildrenOf(map, childNode);
            }
        }
        if (allChildrenAreNowLeaf || N <= 0) {
            break;
        }
    }
    qInfo() << "BR" << N - 1;
    beginRemoveRows(pidToIndex(root.getPID()), 0, std::max(0, N - 1));
    root.clearChildren();
    endRemoveRows();
    qInfo() << "ER" << N - 1;
}

void IProcessItemModel::addAllAsChildrenOf(
    QHash<proc_id_t, ProcessData>& mapBefore,
    ProcessData& root,
    QHash<proc_id_t, ProcessData>& mapAfter,
    QSet<proc_id_t> const& newChildProcIds
) {
    if (newChildProcIds.isEmpty())
        return;

    int base = root.getChildrenProcIds().size();
    int N = newChildProcIds.size();
    for (proc_id_t pid : newChildProcIds) {
        if (!mapAfter.contains(pid) || root.hasChildProcId(pid)) {
            --N;
        }
    }

    if (N > 0) {
        qInfo() << "BIR" << base << base + N;
        beginInsertRows(pidToIndex(root.getPID()), base, base + N - 1);
        for (proc_id_t pid : newChildProcIds) {
            if (!mapAfter.contains(pid)) {
                continue;
            }
            mapBefore[pid] = mapAfter[pid];
            mapBefore[pid].clearChildren();
            root.addChildProcId(pid);
        }
        endInsertRows();
        qInfo() << "EIR" << base << base + N;
    }

    for (proc_id_t pid : newChildProcIds) {
        if (!mapBefore.contains(pid) || !mapAfter.contains(pid)) {
            continue;
        }
        addAllAsChildrenOf(mapBefore, mapBefore[pid], mapAfter, mapAfter[pid].getChildrenProcIdSet());
    }
}

void IProcessItemModel::diffAndMigrate(QHash<proc_id_t, ProcessData>& mapBefore, ProcessData& rootBefore, QHash<proc_id_t, ProcessData>& mapAfter, ProcessData const& rootAfter) {
    QSet<proc_id_t> const& childrenRootBefore = rootBefore.getChildrenProcIdSet();
    QSet<proc_id_t> const& childrenRootAfter = rootAfter.getChildrenProcIdSet();

    QSet<proc_id_t> pidsToAdd = childrenRootBefore - childrenRootAfter;
    QSet<proc_id_t> pidsToRemove = childrenRootAfter - childrenRootBefore;
    QSet<proc_id_t> pidsToModify{childrenRootBefore};
    pidsToModify.intersect(childrenRootAfter);

    for (auto it = pidsToModify.begin(); it != pidsToModify.end();) {
        proc_id_t pid = *it;
        // Edge case:
        // If ppid changes then we actually need to remove and add it again!
        if (!mapBefore.contains(pid) || !mapAfter.contains(pid)) {
            throw std::runtime_error{"this should never happen"};
        }
        ProcessData& rowBefore = mapBefore[pid];
        ProcessData& rowAfter = mapAfter[pid];
        if (rowAfter.getPPID() != rowBefore.getPPID()) {
            pidsToModify.erase(it);
            pidsToRemove.insert(pid);
            pidsToAdd.insert(pid);
        } else {
            ++it;
        }
    }

    // 1. REMOVE
    for (proc_id_t pid : pidsToRemove) {
        if (!mapBefore.contains(pid)) {
            continue;
        }
        ProcessData& rowToRemove = mapBefore[pid];
        removeAllChildrenOf(mapBefore, rowToRemove);
        int positionOfPidToRemove = rootBefore.getChildrenProcIds().indexOf(pid);
        if (positionOfPidToRemove >= 0) {
            qInfo() << "BR2" << positionOfPidToRemove;
            beginRemoveRows(pidToIndex(rootBefore.getPID()), positionOfPidToRemove, positionOfPidToRemove);
            rowToRemove.removeChildProcId(pid);
            endRemoveRows();
            qInfo() << "ER2" << positionOfPidToRemove;
        }
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

void IProcessItemModel::rebuildTreeOnUI(QHash<proc_id_t, ProcessData> newMap, proc_count_t numProcs) {
    QMutexLocker lock{&m_pidToProcessDataMapWriteMutex};

    m_numProcs = numProcs;

    proc_id_t const imaginaryRootPid = getImaginaryRootProcId();

    if (!m_pidToProcessDataMap.contains(imaginaryRootPid)) {
        // Rebuild entirely, since even the root is not there !beginResetModel();
        beginResetModel();
        m_pidToProcessDataMap.clear();
        m_pidToProcessDataMap = newMap;
        // ProcessData proc;
        // proc.setPPID(getImaginaryRootProcId());
        // proc.setPID(getImaginaryRootProcId());
        // proc.setFieldValue(1, "Root");
        // m_pidToProcessDataMap.insert(proc.getPID(), proc);
        //
        // proc.setPPID(getImaginaryRootProcId());
        // proc.setPID(1);
        // proc.setFieldValue(1, "HELLO");
        // m_pidToProcessDataMap.insert(proc.getPID(), proc);
        //
        // proc.setPID(2);
        // proc.setFieldValue(1, "GOOD BYE");
        // m_pidToProcessDataMap.insert(proc.getPID(), proc);
        //
        // proc.setPPID(2);
        // proc.setPID(3);
        // proc.setFieldValue(1, "Child");
        // m_pidToProcessDataMap.insert(proc.getPID(), proc);
        //
        // m_pidToProcessDataMap[getImaginaryRootProcId()].addChildProcId(1);
        // m_pidToProcessDataMap[getImaginaryRootProcId()].addChildProcId(2);
        // m_pidToProcessDataMap[2].addChildProcId(3);
        endResetModel();
    } else {
        diffAndMigrate(m_pidToProcessDataMap, m_pidToProcessDataMap[imaginaryRootPid], newMap, newMap[imaginaryRootPid]);
        emit rebuildTreeComplete(numProcs);
    }

    // Step 1: Remove missing items
    // for (proc_id_t pid : removedPids) {
    //     if (!m_pidToProcessDataMap.contains(pid)) {
    //         continue;
    //     }
    //     proc_id_t ppid = m_pidToProcessDataMap[pid].getPPID();
    //     if (m_pidToProcessDataMap.contains(ppid)) {
    //         ProcessData& parentProcessData = m_pidToProcessDataMap[ppid];
    //         const auto& siblings = parentProcessData.getChildrenProcIds();
    //         int row = siblings.indexOf(pid);

    //         if (row >= 0) {
    //             QModelIndex parentIndex = pidToIndex(ppid); // your helper
    //             qInfo() << "BIR 0" << parentIndex.internalId() << row;
    //             beginRemoveRows(parentIndex, row, row);
    //             parentProcessData.removeChildProcId(pid);
    //             endRemoveRows();
    //         }
    //     }
    //     m_pidToProcessDataMap.remove(pid);
    // }

    // m_pidToProcessDataMap[imaginaryRootPid] = newMap[imaginaryRootPid];

    // // Step 2: Add new items
    // for (proc_id_t pid : addedPids) {
    //     proc_id_t ppid = newMap[pid].getPPID(); // always in newMap since addedPids = newPids - oldPids

    //     if (!m_pidToProcessDataMap.contains(ppid)) {
    //         m_pidToProcessDataMap.insert(ppid, newMap[ppid]);
    //     }

    //     ProcessData& parent = m_pidToProcessDataMap[ppid];
    //     int insertRow = parent.getChildrenProcIds().size(); // insert at end

    //     QModelIndex parentIndex = pidToIndex(ppid);
    //     qInfo() << "BIR 1" << parentIndex.internalId() << insertRow << parentIndex.isValid();
    //     beginInsertRows(parentIndex, insertRow, insertRow);
    //     parent.addChildProcId(pid);
    //     m_pidToProcessDataMap.insert(pid, newMap[pid]);
    //     endInsertRows();
    // }

    // // Step 3: Modify updated items
    // for (proc_id_t pid : newPids) {
    //     if (!addedPids.contains(pid)) {
    //         proc_id_t oldPPID = m_pidToProcessDataMap[pid].getPPID();
    //         proc_id_t newPPID = newMap[pid].getPPID();
    //         if (oldPPID != newPPID) {
    //             // change parent => remove and add again!
    //             // Simulate remove from old parent
    //             int oldRow = m_pidToProcessDataMap[oldPPID].getChildrenProcIds().indexOf(pid);
    //             if (oldRow >= 0) {
    //                 beginRemoveRows(pidToIndex(oldPPID), oldRow, oldRow);
    //                 m_pidToProcessDataMap[oldPPID].removeChildProcId(pid);
    //                 endRemoveRows();
    //             }

    //             // Simulate insert to new parent
    //             if (!m_pidToProcessDataMap.contains(newPPID)) {
    //                 m_pidToProcessDataMap.insert(newPPID, newMap[newPPID]);
    //             }
    //             int newRow = m_pidToProcessDataMap[newPPID].getChildrenProcIds().size();
    //             qInfo() << "BIR 2" << pidToIndex(newPPID).internalId() << newRow;
    //             beginInsertRows(pidToIndex(newPPID), newRow, newRow);
    //             m_pidToProcessDataMap[newPPID].addChildProcId(pid);
    //             endInsertRows();
    //             m_pidToProcessDataMap[pid] = newMap[pid];
    //         } else {
    //             // Always assuming the row has changed
    //             // (since it'd be inefficient to compare to know which is changed and which is not)
    //             m_pidToProcessDataMap[pid] = newMap[pid];
    //             QModelIndex changedIndex = pidToIndex(pid);
    //             emit dataChanged(
    //                 changedIndex.siblingAtColumn(0),
    //                 changedIndex.siblingAtColumn(columnCount() - 1)
    //             );
    //         }
    //     }
    // }

    // // Optional Step 4: Cleanup leftover dummy parents
    // QList<proc_id_t> staleDummies;
    // for (auto it = m_pidToProcessDataMap.begin(); it != m_pidToProcessDataMap.end(); ++it) {
    //     proc_id_t pid = it.key();
    //     if (pid == imaginaryRootPid)
    //         continue;

    //     const ProcessData& proc = it.value();
    //     bool neverUpdated = !newMap.contains(pid);
    //     bool hasNoChildren = proc.getChildrenProcIds().isEmpty();

    //     if (neverUpdated && hasNoChildren) {
    //         staleDummies.append(pid);
    //     }
    // }
    // // Actually remove them
    // for (proc_id_t pid : staleDummies) {
    //     proc_id_t ppid = m_pidToProcessDataMap[pid].getPPID();

    //     if (m_pidToProcessDataMap.contains(ppid)) {
    //         ProcessData& parent = m_pidToProcessDataMap[ppid];
    //         int row = parent.getChildrenProcIds().indexOf(pid);
    //         if (row >= 0) {
    //             QModelIndex parentIndex = pidToIndex(ppid);
    //             beginRemoveRows(parentIndex, row, row);
    //             parent.removeChildProcId(pid);
    //             endRemoveRows();
    //         }
    //     }

    //     m_pidToProcessDataMap.remove(pid);
    // }
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

    ProcessData root;
    root.setPID(imaginaryRootPid);
    root.setPPID(imaginaryRootPid);
    root.clearChildren();
    newMap.insert(imaginaryRootPid, root);

    proc_count_t N = 0;
    ProcessData proc;

    while (getNext(proc)) {
        ++N;
        proc_id_t pid = proc.getPID();
        proc_id_t ppid = proc.getPPID();

        // Defensive sanity check (skip pathological entry)
        if (pid != imaginaryRootPid && pid == ppid) {
            qWarning() << "process " << pid << " has itself as parent, skipping";
            continue;
        }

        bool ok;
        validateIds(pid, ppid, ok);
        if (!ok) {
            continue;
        }

        if (!newMap.contains(ppid)) {
            ProcessData dummyParent;
            dummyParent.setPID(ppid);
            dummyParent.setPPID(imaginaryRootPid);
            dummyParent.clearChildren();
            newMap.insert(ppid, dummyParent);
        }

        newMap[pid] = proc;
        newMap[ppid].addChildProcId(pid);
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
