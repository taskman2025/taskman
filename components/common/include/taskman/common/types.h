#ifndef TASKMAN_COMMON_TYPES_INCLUDED
#define TASKMAN_COMMON_TYPES_INCLUDED

#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef int64_t proc_id_t;
    #define FORMAT_SPECIFIER_OF_proc_id_t PRId64
    typedef proc_id_t proc_count_t;

    typedef uint64_t field_mask_t;
    typedef uint64_t filter_param_type_mask_t;
    typedef int16_t filter_type_id_t;
    typedef int process_action_id_t;

    typedef uint64_t timestamp_t;

    typedef uint64_t backend_request_id_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TASKMAN_COMMON_TYPES_INCLUDED
