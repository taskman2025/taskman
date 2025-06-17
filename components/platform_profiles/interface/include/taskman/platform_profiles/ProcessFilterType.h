#ifndef ProcessFilterType_INCLUDED
#define ProcessFilterType_INCLUDED

#include <QString>
#include "taskman/common/types.h"
#include "taskman/common/bit_ops.h"
#include <QList>

struct ProcessFilterType {
    filter_type_id_t id;
    QString name;
    QString description;
    QList<filter_param_type_mask_t> parameters;
};

#endif // ProcessFilterType_INCLUDED
