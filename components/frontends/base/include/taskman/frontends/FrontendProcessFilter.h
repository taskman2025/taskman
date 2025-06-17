#ifndef FrontendProcessFilter_INCLUDED
#define FrontendProcessFilter_INCLUDED

#include "taskman/platform_runtimes/IProcessFilter.h"
#include "taskman/platform_profiles/ProcessFilterType.h"
#include "taskman/backends/ProcessData.h"
#include <QHash>

constexpr filter_type_id_t const FRONTEND_PROCESS_FILTER = static_cast<filter_type_id_t>(-1);

/**
 * Primarily filter by text.
 * MUST be run in the main (UI) thread!
 */
class FrontendProcessFilter : public IProcessFilter {
public:
    static ProcessFilterType const FILTER_TYPE;
    FrontendProcessFilter(QList<QList<QVariant>> const& argsList, QHash<proc_id_t, ProcessData> const map);
    virtual ~FrontendProcessFilter();

    virtual filter_type_id_t getFilterTypeId() const override;
    virtual qint64 getFilterResultStaleTimeInSeconds() const override;
    virtual qint64 getArgumentsUpdateIntervalInSeconds() const override;

protected:
    virtual FilterError doApply(QSet<proc_id_t>& pidSet, QString& errorMessage) override;
    virtual FilterError doUpdateArguments(QString& error) override;
    virtual FilterError doDigestArguments(QList<QList<QVariant>> const& argsList) override;
    virtual void doMerge(IProcessFilter const& other) override;

private:
    QHash<field_mask_t, QSet<QString>> m_textMatchers;
    QHash<proc_id_t, ProcessData> const m_map;
};

#endif // FrontendProcessFilter_INCLUDED
