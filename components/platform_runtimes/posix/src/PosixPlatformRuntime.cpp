#include "taskman/platform_runtimes/PosixPlatformRuntime.h"
#include "taskman/platform_profiles/PosixPlatformProfile.h"
#include "taskman/platform_runtimes/posix/PosixProcessFilter.h"
#include "taskman/platform_runtimes/posix/PosixProcessReader.h"
#include "taskman/platform_runtimes/posix/PosixActionRunner.h"

#include <QFile>
#include <QSysInfo>
#include <grp.h>
#include <pwd.h>
#include <sys/utsname.h>

PosixPlatformRuntime& PosixPlatformRuntime::instance() {
    static PosixPlatformRuntime inst;
    return inst;
}


PosixPlatformRuntime::PosixPlatformRuntime()
    : IPlatformRuntime{PosixPlatformProfile::instance()},
      m_userNameCache(256), m_groupNameCache(256),
      m_machineType{}, m_osFamilyName{}, m_osName{}, m_osVersion{} {
    struct utsname buf;
    if (uname(&buf) == 0) {
        m_machineType = buf.machine;
        m_kernelVersion = buf.release;
        m_osFamilyName = buf.sysname;
    } else {
        qWarning() << "uname() system call failed";
    }

    QFile file("/etc/os-release");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open /etc/os-release";
    } else {
        QString name, version;
        QTextStream in(&file);
        while (!in.atEnd()) {
            if (!name.isEmpty() && !version.isEmpty()) {
                break;
            }
            QString line = in.readLine();
            if (line.startsWith("NAME=") && name.isEmpty()) {
                name = line.mid(5).remove('"'); // 5 = strlen("NAME=")
            } else if (line.startsWith("VERSION=") && version.isEmpty()) {
                version = line.mid(8).remove('"'); // 8 = strlen("VERSION=")
            }
        }
        m_osName = name;
        m_osVersion = version;
    }
}

PosixPlatformRuntime::~PosixPlatformRuntime() = default;

QString PosixPlatformRuntime::getMachineType() {
    return m_machineType.isEmpty()
               ? IPlatformRuntime::getMachineType()
               : m_machineType;
}

QString PosixPlatformRuntime::getOSKernelVersion() {
    return m_kernelVersion.isEmpty()
               ? IPlatformRuntime::getOSKernelVersion()
               : m_kernelVersion;
}

QString PosixPlatformRuntime::getOSFamilyName() {
    return m_osFamilyName.isEmpty()
               ? "*nix/posix"
               : m_osFamilyName;
}

QString PosixPlatformRuntime::getOSName() {
    return m_osName.isEmpty()
               ? IPlatformRuntime::getOSName()
               : m_osName;
}

QString PosixPlatformRuntime::getOSVersion() {
    return m_osVersion.isEmpty()
               ? IPlatformRuntime::getOSVersion()
               : m_osVersion;
}

IProcessReader* PosixPlatformRuntime::startReadingProcesses(field_mask_t fields) {
    return new PosixProcessReader(*this, fields);
}

IProcessFilter* PosixPlatformRuntime::createFilter(
    filter_type_id_t filterTypeId,
    QList<QList<QVariant>> const& argsList
) {
    return PosixProcessFilter::createInternalFilter(filterTypeId, argsList);
}

ActionResult PosixPlatformRuntime::applyAction(process_action_id_t actionId, proc_id_t pid) {
    return PosixActionRunner(*this).run(actionId, pid);
}

QString PosixPlatformRuntime::getUserNameById(uint64_t userId) {
    if (auto cached = m_userNameCache.object(userId)) {
        return *cached;
    }

    struct passwd* pw = getpwuid(userId);
    QString name = pw ? QString::fromLocal8Bit(pw->pw_name) : "(" + QString::number(userId) + ")";

    m_userNameCache.insert(userId, new QString(name));
    return name;
}

QString PosixPlatformRuntime::getGroupNameById(uint64_t groupId) {
    if (auto cached = m_groupNameCache.object(groupId)) {
        return *cached;
    }

    struct group* gr = getgrgid(groupId);
    QString name = gr ? QString::fromLocal8Bit(gr->gr_name) : "(" + QString::number(groupId) + ")";

    m_groupNameCache.insert(groupId, new QString(name));
    return name;
}
