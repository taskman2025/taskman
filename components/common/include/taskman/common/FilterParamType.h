#ifndef FilterParamType_INCLUDED
#define FilterParamType_INCLUDED

#include "taskman/common/types.h"
#include "taskman/common/declaration.h"

#define DECLARE_FILTER_PARAM_TYPE_BIT(name, pos) DECLARE_BIT(filter_param_type_mask_t, name, pos)

class FilterParamType {
public:
    DECLARE_FILTER_PARAM_TYPE_BIT(TEXT, 1)
    DECLARE_FILTER_PARAM_TYPE_BIT(EXISTING_FILE_PATH, 2)
    DECLARE_FILTER_PARAM_TYPE_BIT(PROCESS_FIELD, 3)
};

#endif // FilterParamType_INCLUDED
