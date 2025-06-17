#ifndef ProcessField_INCLUDED
#define ProcessField_INCLUDED

#include <cstdint>
#include <QString>
#include "taskman/common/types.h"
#include "taskman/common/bit_ops.h"

struct ProcessField {
    QString name;
    QString description;
    field_mask_t mask;
    /**
     * Size of this field in the raw process data, in bytes.
     * If size == 0 then this field's size varies, not fixed.
     */
    size_t size;

    inline bool isPresentIn(field_mask_t value) const {
        return hasAny(value, mask);
    }
};

#endif // ProcessField_INCLUDED
