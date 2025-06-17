#include "taskman/platform_runtimes/posix/process_filters/PosixOpenFileProcessFilter.h"
#include "taskman/platform_profiles/posix/process_filter_type_ids.h"
#include <cassert>
#include <dirent.h>

PosixOpenFileProcessFilter::PosixOpenFileProcessFilter(QList<QList<QVariant>> const& argsList)
    : PosixBaseProcessFilter(argsList), m_targetFileDevAndInodePairs{}, m_targetFilePaths{} {}

PosixOpenFileProcessFilter::~PosixOpenFileProcessFilter() = default;

filter_type_id_t PosixOpenFileProcessFilter::getFilterTypeId() const {
    return PosixProcessFilterTypeID::OPEN_FILE;
}

qint64 PosixOpenFileProcessFilter::getFilterResultStaleTimeInSeconds() const {
    return 6;
}

qint64 PosixOpenFileProcessFilter::getArgumentsUpdateIntervalInSeconds() const {
    return 15;
}

FilterError PosixOpenFileProcessFilter::doApply(QSet<proc_id_t>& pidSet, QString& errorMessage) {
    errorMessage.clear();
#define FILE_PATH_BUF_SIZE (sizeof("/proc//fd") + 20) // 64 bit means maximum 20 digits
    char fdDirPath[FILE_PATH_BUF_SIZE];
    for (auto iter = pidSet.begin(); iter != pidSet.end(); ) {
        proc_id_t pid = *iter;
        sprintf(fdDirPath, "/proc/%" FORMAT_SPECIFIER_OF_proc_id_t "/fd", pid);
        DIR* fdDir = opendir(fdDirPath);
        if (NULL == fdDir) {
            errorMessage = QString("cannot open directory ") + fdDirPath + " ; errno = " + QString::number(errno);
            if (EACCES == errno) {
                return FilterError::PERMISSION_DENIED;
            }
            return FilterError::OTHER;
        }

        struct dirent* fdEntry;
        struct stat fdStat;

        bool found = false;
        while ((fdEntry = readdir(fdDir)) != NULL) {
            if (0 == strcmp(".", fdEntry->d_name) || 0 == strcmp("..", fdEntry->d_name)) {
                continue;
            }
            if (stat(fdEntry->d_name, &fdStat) == -1) {
                if (EACCES == errno) {
                    errorMessage = QString("cannot stat file ") + fdEntry->d_name + " ; errno = " + QString::number(errno);
                    return FilterError::PERMISSION_DENIED;
                }
                continue;
            }
            if (m_targetFileDevAndInodePairs.contains({fdStat.st_dev, fdStat.st_ino})) {
                found = true;
                break;
            }
        }

        closedir(fdDir);
        if (found) {
            ++iter;
        } else {
            iter = pidSet.erase(iter);
        }
    }
    return FilterError::NONE;
}

FilterError PosixOpenFileProcessFilter::doUpdateArguments(QString& error) {
    m_targetFileDevAndInodePairs.clear();
    error.clear();
    FilterError err = FilterError::NONE;

    for (QString const& targetFilePath : m_targetFilePaths) {
        struct stat targetFileStat;
        if (stat(targetFilePath.toUtf8().constData(), &targetFileStat) == -1) {
            error = "stat failed on file: " + targetFilePath + "\n";
            err = FilterError::TRIVIAL;
            continue;
        }
        m_targetFileDevAndInodePairs.insert({targetFileStat.st_dev, targetFileStat.st_ino});
    }

    error = error.trimmed();
    return err;
}

void PosixOpenFileProcessFilter::doMerge(IProcessFilter const& otherBase) {
    PosixOpenFileProcessFilter const& other
        = dynamic_cast<PosixOpenFileProcessFilter const&>(otherBase);
    
    m_targetFileDevAndInodePairs.unite(other.m_targetFileDevAndInodePairs);
    m_targetFilePaths.append(other.m_targetFilePaths);
}

FilterError PosixOpenFileProcessFilter::doDigestArguments(
    QList<QList<QVariant>> const& argsList
) {
    for (QList<QVariant> const& args : argsList) {
        assert(args.size() >= 1);
        assert(args[0].canConvert<QString>());
        QString targetFilePath = args[0].toString();
        m_targetFilePaths.append(targetFilePath);
    }
    return FilterError::NONE;
}
