#include "taskman/platform_runtimes/IProcessFilter.h"
#include <cassert>

IProcessFilter::IProcessFilter(QList<QList<QVariant>> const& argsList)
    : m_lastArgumentsUpdate{}, m_neverUpdatedArguments{true},
      m_neverDigestedArguments{true}, m_tempArgsList{argsList} {}

IProcessFilter::~IProcessFilter() = default;

FilterError IProcessFilter::apply(QSet<proc_id_t>& pidSet, QString& errorMessage) {
    FilterError err = updateArgumentsAsNeeded(errorMessage);
    if (FilterError::NONE == err || FilterError::TRIVIAL == err) {
        return doApply(pidSet, errorMessage);
    }
    return err;
}

IProcessFilter& IProcessFilter::operator+=(IProcessFilter const& other) {
    assert(getFilterTypeId() == other.getFilterTypeId());
    doMerge(other);
    return *this;
}

FilterError IProcessFilter::updateArgumentsAsNeeded(QString& error) {
    digestArgumentsAsNeeded();
    QDateTime previousUpdateTime = m_lastArgumentsUpdate;
    m_lastArgumentsUpdate = QDateTime::currentDateTime();
    QDateTime& currentUpdateTime = m_lastArgumentsUpdate;

    if (m_neverUpdatedArguments) {
        m_neverUpdatedArguments = false;
    } else {
        if (m_lastArgumentsUpdateError == FilterError::NONE) {
            if (previousUpdateTime.secsTo(currentUpdateTime) < getArgumentsUpdateIntervalInSeconds()) {
                return FilterError::NONE;
            }
        }
    }

    m_lastArgumentsUpdateError = doUpdateArguments(error);
    return m_lastArgumentsUpdateError;
}

FilterError IProcessFilter::updateArgumentsNow(QString& error) {
    digestArgumentsAsNeeded();
    m_lastArgumentsUpdate = QDateTime::currentDateTime();
    m_lastArgumentsUpdateError = doUpdateArguments(error);
    return m_lastArgumentsUpdateError;
}

void IProcessFilter::setArgsList(QList<QList<QVariant>> const& newArgsList) {
    m_neverUpdatedArguments = true;
    doDigestArguments(newArgsList);
}

void IProcessFilter::digestArgumentsAsNeeded() {
    if (m_neverDigestedArguments) {
        m_neverDigestedArguments = false;
        doDigestArguments(m_tempArgsList);
        m_tempArgsList.clear();
    }
}
