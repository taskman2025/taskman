#include "taskman/platform/PosixPlatform.h"
#include "taskman/platform/PosixCommonDefs.h"
#include "taskman/platform/PosixProcessFilter.h"
#include "taskman/platform/PosixProcessReader.h"
#include <QFile>
#include <QString>
#include <grp.h>
#include <pwd.h>

QList<ProcessField> const PosixPlatform::PROCESS_FIELDS = {
    {.name = "Name",
     .description = "The process name. By default, it is the filename of the executable. A running process may alter this name by calling `prctl(PR_SET_NAME, \"new name\");`. See `man 2 prctl`",
     .mask = FieldBit::NAME,
     .size = 32},

    {.name = "PID",
     .description = "ID of the process.",
     .mask = FieldBit::PID,
     .size = 8},

    {.name = "PPID",
     .description = "ID of the parent process.",
     .mask = FieldBit::PPID,
     .size = 8},

    {.name = "State",
     .description = "Process state, e.g. R = Running, S = Sleeping, Z = Zombie, etc.",
     .mask = FieldBit::STATE,
     .size = 1},

    {.name = "pgrp",
     .description = "Process group ID (used for terminal control and job control).",
     .mask = FieldBit::PGRP,
     .size = 4},

    {.name = "nice",
     .description = "Nice value of a process, ranging from -20 (least nice, or highest priority) to 19 (nicest, or lowest priority).",
     .mask = FieldBit::NICE,
     .size = 1},

    {.name = "starttime",
     .description = "Time when the process started after system boot, in clock ticks. This is not human-readable.",
     .mask = FieldBit::STARTTIME,
     .size = 1},

    {.name = "Real UID",
     .description = "ID of the user who started the process.",
     .mask = FieldBit::REAL_UID,
     .size = 4},

    {.name = "Effective UID",
     .description = "Effective user ID, used for permissions e.g. file access.",
     .mask = FieldBit::EFFECTIVE_UID,
     .size = 4},

    {.name = "Real GID",
     .description = "ID of the group of the user who started the process.",
     .mask = FieldBit::REAL_GID,
     .size = 4},

    {.name = "Effective GID",
     .description = "Effective group ID, used for permissions e.g. file access.",
     .mask = FieldBit::EFFECTIVE_GID,
     .size = 4},

    {.name = "Real User",
     .description = "Name of the user who started the process. This corresponds to Real UID.",
     .mask = FieldBit::REAL_USER,
     .size = USER_NAME_BUFFER_LENGTH},

    {.name = "Effective User",
     .description = "Name of the user that is used for permissions e.g. file access. This corresponds to Effective UID",
     .mask = FieldBit::EFFECTIVE_USER,
     .size = USER_NAME_BUFFER_LENGTH},

    {.name = "Real Group",
     .description = "Name of the group of the user who started the process. This corresponds to Real GID.",
     .mask = FieldBit::REAL_GROUP,
     .size = GROUP_NAME_BUFFER_LENGTH},

    {.name = "Effective Group",
     .description = "Name of the effective group, used for permissions e.g. file access. This corresponds to Effective GID.",
     .mask = FieldBit::EFFECTIVE_GROUP,
     .size = GROUP_NAME_BUFFER_LENGTH},

    {.name = "Command line",
     .description = "The command line used to start the process.",
     .mask = FieldBit::COMMAND_LINE,
     .size = 0},

    {.name = "Advanced Filter Check",
     .description = "This column will be marked and will highlight the whole row if the row satisfies the given advanced filters. One kind of advanced filter is the Open-File process filter, which highlights processes that are opening a particular file.",
     .mask = FieldBit::FILTERED,
     .size = 1}
};

PosixPlatform::PosixPlatform() : m_userCache(1000), m_groupCache(1000) {
    QFile file("/etc/os-release");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open /etc/os-release";
        m_osFamilyName = "UNIX";
        m_osName = "Unknown OS";
        m_osVersion = "unknown OS version";
        return;
    }

    QString name, version;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("NAME=") && name.isEmpty()) {
            name = line.mid(5).remove('"');
        } else if (line.startsWith("VERSION=") && version.isEmpty()) {
            version = line.mid(8).remove('"');
        }
    }

    m_osFamilyName = "UNIX";
    m_osName = name;
    m_osVersion = version;
}

QList<ProcessField> const& PosixPlatform::getProcessFields() const {
    return PROCESS_FIELDS;
}

QString const& PosixPlatform::getPlatformName() {
    return PLATFORM_NAME;
}

IProcessReader* PosixPlatform::startReadingProcesses(ReadProcessesRequest req) {
    return new PosixProcessReader(*this, req.fields, req.filters);
}

IProcessFilter* PosixPlatform::createProcessFilter(field_mask_t filterBy, QVariant arg) {
    switch (filterBy) {
    case FilterBit::OPEN_FILE: {
        QString filePath = arg.value<QString>();
        return new PosixProcessOpenFileFilter(*this, filePath);
    }
    default:
        return new NonApplicableProcessFilter();
    }
}

QString const PosixPlatform::PLATFORM_NAME = "POSIX";

QString const& PosixPlatform::getOSFamilyName() {
    return m_osFamilyName;
}

QString const& PosixPlatform::getOSName() {
    return m_osName;
}

QString const& PosixPlatform::getOSVersion() {
    return m_osVersion;
}

QString PosixPlatform::getUserNameById(uint64_t userId) {
    if (auto cached = m_userCache.object(userId)) {
        return *cached;
    }

    struct passwd* pw = getpwuid(userId);
    QString name = pw ? QString::fromLocal8Bit(pw->pw_name) : "(" + QString::number(userId) + ")";

    m_userCache.insert(userId, new QString(name));
    return name;
}

QString PosixPlatform::getGroupNameById(uint64_t groupId) {
    if (auto cached = m_groupCache.object(groupId)) {
        return *cached;
    }

    struct group* gr = getgrgid(groupId);
    QString name = gr ? QString::fromLocal8Bit(gr->gr_name) : "(" + QString::number(groupId) + ")";

    m_groupCache.insert(groupId, new QString(name));
    return name;
}
