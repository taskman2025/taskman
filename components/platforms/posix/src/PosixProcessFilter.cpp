#include "taskman/platform/PosixProcessFilter.h"

BasePosixProcessFilter::~BasePosixProcessFilter() {}

PosixProcessOpenFileFilter::~PosixProcessOpenFileFilter() {}

PosixProcessOpenFileFilter::PosixProcessOpenFileFilter(PosixPlatform const& platform, QString filePath)
    : m_platform{platform} {
    struct stat fileStat;
    if (stat(filePath.toUtf8().constData(), &fileStat) == -1) {
        m_applicable = false;
        m_targetFileDev = m_targetFileInode = 0;
    } else {
        m_applicable = true;
        m_targetFileDev = static_cast<uint64_t>(fileStat.st_dev);
        m_targetFileInode = static_cast<uint64_t>(fileStat.st_ino);
    }
}

bool PosixProcessOpenFileFilter::applyToCurrent(IProcessReader const& reader) const {
    proc_id_t pid = reader.getCurrentProcId();
    QString fdsPath = QString{"/proc/%1/fd"}.arg(pid);

    DIR* fdDir = opendir(fdsPath.toUtf8().constData());
    if (fdDir == NULL) {
        if (errno == EACCES) {
            // TODO: report permission error and demand privileges
        }
        return false;
    }

    struct dirent* fdEntry;
    struct stat fdStat;
    dev_t targetDev = static_cast<dev_t>(m_targetFileDev);
    ino_t targetInode = static_cast<ino_t>(m_targetFileInode);
    bool found = false;
    while ((fdEntry = readdir(fdDir)) != NULL) {
        if (0 == strcmp(".", fdEntry->d_name) || 0 == strcmp("..", fdEntry->d_name)) {
            continue;
        }
        if (stat(fdEntry->d_name, &fdStat) == -1) {
            continue;
        }
        if (targetDev == fdStat.st_dev && targetInode == fdStat.st_ino) {
            found = true;
            break;
        }
    }

    closedir(fdDir);
    return found;
}

bool PosixProcessOpenFileFilter::doIsEqualForSameFilterTypeAndApplicable(IProcessFilter const& abstractOther) const {
    PosixProcessOpenFileFilter const* other = dynamic_cast<PosixProcessOpenFileFilter const*>(&abstractOther);
    return (
        &m_platform == &other->m_platform && m_targetFileDev == other->m_targetFileDev && m_targetFileInode == other->m_targetFileInode
    );
}
