#ifndef DefaultProcessDataStruct_INCLUDED
#define DefaultProcessDataStruct_INCLUDED

#include "taskman/common/types.h"
#include <QList>
#include <QHash>
#include <QVariant>

class ProcessData {
private:
    proc_id_t pid, ppid;
    QList<proc_id_t> m_childrenProcIds;
    QSet<proc_id_t> m_childrenProcIdSet;
    QHash<field_mask_t, QVariant> m_data;

public:
    ProcessData() = default;

    ProcessData& operator=(ProcessData const& other);
    void update(ProcessData const& other);

    proc_id_t getPID() const { return pid; }
    proc_id_t getPPID() const { return ppid; }
    void setPID(proc_id_t newPID) { pid = newPID; }
    void setPPID(proc_id_t newPPID) { ppid = newPPID; }
    QVariant getFieldValue(field_mask_t fieldBit) const { return m_data[fieldBit]; }
    void setFieldValue(field_mask_t fieldBit, QVariant value) { m_data[fieldBit] = value; }
    QList<proc_id_t> const& getChildrenProcIds() const { return m_childrenProcIds; }
    QSet<proc_id_t> const& getChildrenProcIdSet() const { return m_childrenProcIdSet; }
    
    void addChildProcId(proc_id_t childProcId);
    void addMultipleChildProcIds(QList<proc_id_t> const& childProcIds);
    void removeChildProcId(proc_id_t childProcId);
    void clearChildren();
    bool hasChildProcId(proc_id_t childProcId);
};

Q_DECLARE_METATYPE(ProcessData);

#endif // DefaultProcessDataStruct_INCLUDED
