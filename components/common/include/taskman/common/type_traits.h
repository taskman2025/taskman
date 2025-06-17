#ifndef TASKMAN_TYPE_TRAITS_INCLUDED
#define TASKMAN_TYPE_TRAITS_INCLUDED

#include <type_traits>

template<typename T>
struct is_unsigned_integral {
    static constexpr bool value = std::is_integral_v<T> && std::is_unsigned_v<T>;
};

template<typename T>
inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

#endif // TASKMAN_TYPE_TRAITS_INCLUDED
