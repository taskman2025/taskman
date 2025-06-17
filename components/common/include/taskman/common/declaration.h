#ifndef DECLARE_BIT_INCLUDED
#define DECLARE_BIT_INCLUDED

#include "taskman/common/type_traits.h"
#include "taskman/common/types.h"

template<typename T, typename std::enable_if_t<is_unsigned_integral_v<T>, bool> = true>
constexpr T bitmask(unsigned bit_position) {
    return T(1) << bit_position;
}

#define DECLARE_BIT(T, name, pos) static T const name = bitmask<T>(pos);

#define DECLARE_FIELD_BIT(name, pos) DECLARE_BIT(field_mask_t, name, pos)
#define DECLARE_FILTER_PARAM_TYPE_BIT(name, pos) DECLARE_BIT(filter_param_type_mask_t, name, pos)

#define DECLARE_PROCESS_FILTER_TYPE_ID(name, value) static filter_type_id_t const name = value;
#define DECLARE_PROCESS_ACTION_ID(name, value) static process_action_id_t const name = value;

#endif // DECLARE_BIT_INCLUDED
