#ifndef PosixOpenFileProcessFilter_INCLUDED
#define PosixOpenFileProcessFilter_INCLUDED

#include "taskman/platform_runtimes/posix/process_filters/PosixBaseProcessFilter.h"
#include <QDateTime>
#include <sys/stat.h>

class PosixOpenFileProcessFilter : public PosixBaseProcessFilter {
public:
    PosixOpenFileProcessFilter(QList<QList<QVariant>> const& argsList);
    virtual ~PosixOpenFileProcessFilter() override;

    virtual filter_type_id_t getFilterTypeId() const override;
    virtual qint64 getFilterResultStaleTimeInSeconds() const override;
    virtual qint64 getArgumentsUpdateIntervalInSeconds() const override;

protected:
    virtual FilterError doApply(QSet<proc_id_t>& pidSet, QString& errorMessage) override;
    virtual FilterError doUpdateArguments(QString& error) override;
    virtual FilterError doDigestArguments(QList<QList<QVariant>> const& argsList) override;
    virtual void doMerge(IProcessFilter const& other) override;

private:
    QSet<QPair<dev_t, ino_t>> m_targetFileDevAndInodePairs;
    QList<QString> m_targetFilePaths;
    QString m_errorWhenUpdatingArguments;
};

#endif // PosixOpenFileProcessFilter_INCLUDED
