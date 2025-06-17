#include "taskman/backend/ProcessData.h"

void ProcessData::addChildProcId(proc_id_t childProcId) {
    if (m_childrenProcIdSet.contains(childProcId)) {
        return;
    }
    m_childrenProcIdSet.insert(childProcId);
    m_childrenProcIds.append(childProcId);
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

ProcessData& ProcessData::operator=(ProcessData const& other) {
    pid = other.pid;
    ppid = other.ppid;
    m_childrenProcIds = other.m_childrenProcIds;
    m_childrenProcIdSet = other.m_childrenProcIdSet;
    m_data = other.m_data;
    return *this;
}

void ProcessData::update(ProcessData const& other) {
    pid = other.pid;
    ppid = other.ppid;
    m_data = other.m_data;
    
    QSet<proc_id_t> missingProcIdSet = other.m_childrenProcIdSet - this->m_childrenProcIdSet;
    for (proc_id_t otherChildPid : other.m_childrenProcIds) {
        if (missingProcIdSet.contains(otherChildPid)) {
            m_childrenProcIds.append(otherChildPid);
        }
    }
    m_childrenProcIdSet += missingProcIdSet;
}
