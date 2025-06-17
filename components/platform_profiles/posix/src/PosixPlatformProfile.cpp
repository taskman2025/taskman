#include "taskman/platform_profiles/PosixPlatformProfile.h"
#include "taskman/common/FilterParamType.h"
#include "taskman/platform_profiles/posix/constants.h"
#include "taskman/platform_profiles/posix/process_action_ids.h"
#include "taskman/platform_profiles/posix/process_field_bits.h"
#include "taskman/platform_profiles/posix/process_filter_type_ids.h"
#include <QDebug>

PosixPlatformProfile& PosixPlatformProfile::instance() {
    static PosixPlatformProfile inst;
    return inst;
}

QList<ProcessField> const PosixPlatformProfile::PROCESS_FIELDS = {
    {.name = "Name",
     .description = "The process name. By default, it is the filename of the executable. A running process may alter this name by calling `prctl(PR_SET_NAME, \"new name\");`. See `man 2 prctl`",
     .mask = PosixProcessFieldBit::NAME,
     .size = 32},

    {.name = "PID",
     .description = "ID of the process.",
     .mask = PosixProcessFieldBit::PID,
     .size = 8},

    {.name = "PPID",
     .description = "ID of the parent process.",
     .mask = PosixProcessFieldBit::PPID,
     .size = 8},

    {.name = "State",
     .description = "Process state, e.g. R = Running, S = Sleeping, Z = Zombie, etc.",
     .mask = PosixProcessFieldBit::STATE,
     .size = 1},

    {.name = "pgrp",
     .description = "Process group ID (used for terminal control and job control).",
     .mask = PosixProcessFieldBit::PGRP,
     .size = 4},

    {.name = "nice",
     .description = "Nice value of a process, ranging from -20 (least nice, or highest priority) to 19 (nicest, or lowest priority).",
     .mask = PosixProcessFieldBit::NICE,
     .size = 1},

    {.name = "starttime",
     .description = "Time when the process started after system boot, in clock ticks. This is not human-readable.",
     .mask = PosixProcessFieldBit::STARTTIME,
     .size = 1},

    {.name = "Real UID",
     .description = "ID of the user who started the process.",
     .mask = PosixProcessFieldBit::REAL_UID,
     .size = 4},

    {.name = "Effective UID",
     .description = "Effective user ID, used for permissions e.g. file access.",
     .mask = PosixProcessFieldBit::EFFECTIVE_UID,
     .size = 4},

    {.name = "Real GID",
     .description = "ID of the group of the user who started the process.",
     .mask = PosixProcessFieldBit::REAL_GID,
     .size = 4},

    {.name = "Effective GID",
     .description = "Effective group ID, used for permissions e.g. file access.",
     .mask = PosixProcessFieldBit::EFFECTIVE_GID,
     .size = 4},

    {.name = "Real User",
     .description = "Name of the user who started the process. This corresponds to Real UID.",
     .mask = PosixProcessFieldBit::REAL_USER,
     .size = USER_NAME_BUFFER_LENGTH},

    {.name = "Effective User",
     .description = "Name of the user that is used for permissions e.g. file access. This corresponds to Effective UID",
     .mask = PosixProcessFieldBit::EFFECTIVE_USER,
     .size = USER_NAME_BUFFER_LENGTH},

    {.name = "Real Group",
     .description = "Name of the group of the user who started the process. This corresponds to Real GID.",
     .mask = PosixProcessFieldBit::REAL_GROUP,
     .size = GROUP_NAME_BUFFER_LENGTH},

    {.name = "Effective Group",
     .description = "Name of the effective group, used for permissions e.g. file access. This corresponds to Effective GID.",
     .mask = PosixProcessFieldBit::EFFECTIVE_GROUP,
     .size = GROUP_NAME_BUFFER_LENGTH},

    {.name = "Command line",
     .description = "The command line used to start the process.",
     .mask = PosixProcessFieldBit::COMMAND_LINE,
     .size = 0}
};

QList<ProcessFilterType> const PosixPlatformProfile::PROCESS_FILTER_TYPES = {
    {.id = PosixProcessFilterTypeID::OPEN_FILE,
     .name = "Open files",
     .description = "Filter by open files. Any process that are opening the specified file will be shown, otherwise hidden.",
     .parameters = {FilterParamType::EXISTING_FILE_PATH}}
};

QList<ProcessAction> const PosixPlatformProfile::PROCESS_ACTIONS = {
    {.id = PosixProcessActionId::END,
     .name = "End",
     .description = "Send SIGTERM to the selected process",
     .confirmationMessage = "Are you sure to end process \"%1\" with PID \"%2\"? (Doing so will send SIGTERM to the process.)",
     .confirmationMessageArguments = {PosixProcessFieldBit::NAME, PosixProcessFieldBit::PID}},

    {.id = PosixProcessActionId::KILL,
     .name = "Kill",
     .description = "Send SIGKILL to the selected process",
     .confirmationMessage = "Are you sure to kill process \"%1\" with PID \"%2\"? (Doing so will send SIGKILL to the process.)",
     .confirmationMessageArguments = {PosixProcessFieldBit::NAME, PosixProcessFieldBit::PID}}
};

PosixPlatformProfile::PosixPlatformProfile() = default;

PosixPlatformProfile::~PosixPlatformProfile() = default;

QList<ProcessField> const& PosixPlatformProfile::getProcessFields() const {
    return PROCESS_FIELDS;
}

QList<ProcessFilterType> const& PosixPlatformProfile::getProcessFilterTypes() const {
    return PROCESS_FILTER_TYPES;
}

QList<ProcessAction> const& PosixPlatformProfile::getProcessActions() const {
    return PROCESS_ACTIONS;
}

proc_id_t PosixPlatformProfile::getImaginaryRootProcId() const {
    return static_cast<proc_id_t>(-1);
}

void PosixPlatformProfile::validateIds(proc_id_t& pid, proc_id_t& ppid, bool& ok) const {
    ok = true;
    if (pid <= 0 || ppid < 0) {
        qWarning() << "erroneous or stale pid" << pid << "; ppid" << ppid << "; skipping";
        ok = false;
        return;
    }
    if (ppid == 0) {
        ppid = getImaginaryRootProcId();
    }
}
