#ifndef BIT_OPERATIONS_INCLUDED
#define BIT_OPERATIONS_INCLUDED

#include "taskman/common/type_traits.h"

template<typename T, typename std::enable_if_t<is_unsigned_integral_v<T>, bool> = true>
inline bool hasAny(T value, T mask) {
    return (value & mask) != 0;
}

#endif // BIT_OPERATIONS_INCLUDED
