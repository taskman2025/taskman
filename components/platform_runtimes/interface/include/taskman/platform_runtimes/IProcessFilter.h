#ifndef IProcessFilter_INCLUDED
#define IProcessFilter_INCLUDED

#include "taskman/common/types.h"
#include <QDateTime>
#include <QSet>
#include <QVariant>

enum class FilterError {
    NONE,
    TRIVIAL,
    OTHER,
    PERMISSION_DENIED
};

class IProcessFilter {
public:
    IProcessFilter(QList<QList<QVariant>> const& argsList);
    virtual ~IProcessFilter() = 0;
    FilterError apply(QSet<proc_id_t>& pidSet, QString& errorMessage);
    IProcessFilter& operator+=(IProcessFilter const& other);
    void setArgsList(QList<QList<QVariant>> const& newArgsList);

    virtual filter_type_id_t getFilterTypeId() const = 0;
    virtual qint64 getFilterResultStaleTimeInSeconds() const = 0;
    virtual qint64 getArgumentsUpdateIntervalInSeconds() const = 0;

protected:
    virtual FilterError doApply(QSet<proc_id_t>& pidSet, QString& errorMessage) = 0;
    virtual FilterError doUpdateArguments(QString& error) = 0;
    virtual FilterError doDigestArguments(QList<QList<QVariant>> const& argsList) = 0;
    virtual void doMerge(IProcessFilter const& other) = 0;

private:
    bool m_neverDigestedArguments;
    QList<QList<QVariant>> m_tempArgsList;

    QDateTime m_lastArgumentsUpdate;
    bool m_neverUpdatedArguments;
    FilterError m_lastArgumentsUpdateError;

    FilterError updateArgumentsNow(QString& error);
    FilterError updateArgumentsAsNeeded(QString& error);
    void digestArgumentsAsNeeded();
};

#endif // IProcessFilter_INCLUDED
