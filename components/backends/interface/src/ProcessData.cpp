#include "taskman/backend/ProcessData.h"

void ProcessData::addChildProcId(proc_id_t childProcId) {
    if (m_childrenProcIdSet.contains(childProcId)) return;
    m_childrenProcIdSet.insert(childProcId);
    m_childrenProcIds.push_back(childProcId);
}

void ProcessData::addMultipleChildProcIds(QList<proc_id_t> const& childProcIds) {
    for (proc_id_t newChildProcId : childProcIds) {
        addChildProcId(newChildProcId);
    }
}

void ProcessData::removeChildProcId(proc_id_t childProcId) {
    m_childrenProcIds.removeOne(childProcId);
    m_childrenProcIdSet.remove(childProcId);
}

void ProcessData::clearChildren() {
    m_childrenProcIds.clear();
    m_childrenProcIdSet.clear();
}

bool ProcessData::hasChildProcId(proc_id_t childProcId) {
    return m_childrenProcIdSet.contains(childProcId);
}
