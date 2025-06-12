#ifndef ProcessField_INCLUDED
#define ProcessField_INCLUDED

#include <cstdint>
#include <QString>

using field_mask_t = uint64_t;

struct ProcessField {
    QString name;
    QString description;
    field_mask_t mask;
    /**
     * Size of this field in the raw process data, in bytes.
     * If size == 0 then this field's size varies, not fixed.
     */
    size_t size;

    bool isPresentIn(field_mask_t value) const {
        return (value & mask);
    }
};

inline bool hasField(field_mask_t value, field_mask_t bit) {
    return (value & bit);
}

inline bool hasAny(int value, int mask) {
    return (value & mask) != 0;
}

#define FIELD_FILTERED 63

#endif // ProcessField_INCLUDED
