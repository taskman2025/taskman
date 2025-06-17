#ifndef ProcessTreeReset_INCLUDED
#define ProcessTreeReset_INCLUDED

#include "taskman/backends/ProcessData.h"
#include "taskman/common/types.h"
#include <QHash>

struct ProcessTree {
    QHash<proc_id_t, ProcessData> pidToProcessDataMap;
    proc_count_t totalNumProcs{0};
    timestamp_t timestamp{0};
};

#endif // ProcessTreeReset_INCLUDED
