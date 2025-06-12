#ifndef ALL_POSIX_PROCESS_FIELDS_INCLUDED
#define ALL_POSIX_PROCESS_FIELDS_INCLUDED

#include "taskman/platform/ProcessField.h"

constexpr field_mask_t bitmask(unsigned bit_position) {
    return field_mask_t(1) << bit_position;
}

#define DECLARE_BIT(name, pos) static field_mask_t const name = bitmask(pos)

class FieldBit {
public:
    DECLARE_BIT(FILTERED, FIELD_FILTERED);
    DECLARE_BIT(PID, 62);
    DECLARE_BIT(PPID, 61);
    DECLARE_BIT(NAME, 0);
    DECLARE_BIT(STATE, 1);
    DECLARE_BIT(PGRP, 2);
    DECLARE_BIT(NICE, 3);
    DECLARE_BIT(STARTTIME, 4);
    DECLARE_BIT(REAL_UID, 5);
    DECLARE_BIT(EFFECTIVE_UID, 6);
    DECLARE_BIT(REAL_GID, 7);
    DECLARE_BIT(EFFECTIVE_GID, 8);
    DECLARE_BIT(REAL_USER, 9);
    DECLARE_BIT(EFFECTIVE_USER, 10);
    DECLARE_BIT(REAL_GROUP, 11);
    DECLARE_BIT(EFFECTIVE_GROUP, 12);
    DECLARE_BIT(COMMAND_LINE, 13);
};

#define USER_NAME_BUFFER_LENGTH 33
#define GROUP_NAME_BUFFER_LENGTH 33

class FilterBit {
public:
    DECLARE_BIT(OPEN_FILE, 0);
};

#endif // ALL_POSIX_PROCESS_FIELDS_INCLUDED
