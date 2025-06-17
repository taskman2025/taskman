#include "taskman/frontends/FrontendProcessFilter.h"
#include "taskman/common/FilterParamType.h"

ProcessFilterType const FrontendProcessFilter::FILTER_TYPE = {
    .id = FRONTEND_PROCESS_FILTER,
    .name = "Filter by text",
    .description = "Filter by text in the selected column",
    .parameters = {
        FilterParamType::PROCESS_FIELD,
        FilterParamType::TEXT
    }
};

FrontendProcessFilter::FrontendProcessFilter(
    QList<QList<QVariant>> const& argsList,
    QHash<proc_id_t, ProcessData> const map
)
    : IProcessFilter(argsList), m_textMatchers{}, m_map{map} {
}

FrontendProcessFilter::~FrontendProcessFilter() = default;

filter_type_id_t FrontendProcessFilter::getFilterTypeId() const {
    return FRONTEND_PROCESS_FILTER;
}

qint64 FrontendProcessFilter::getFilterResultStaleTimeInSeconds() const {
    return 3;
}

qint64 FrontendProcessFilter::getArgumentsUpdateIntervalInSeconds() const {
    return 10000; // never have to!
}

/**
 * Reverse: this will actually add new pids to pidSet!
 */
FilterError FrontendProcessFilter::doApply(QSet<proc_id_t>& pidSet, QString& errorMessage) {
    errorMessage.clear();
    for (auto iter = m_map.cbegin(); iter != m_map.cend(); ++iter) {
        proc_id_t pid = iter.key();
        ProcessData const& data = iter.value();
        bool pass = false;

        for (auto iterMatcher = m_textMatchers.cbegin(); iterMatcher != m_textMatchers.cend(); ++iterMatcher) {
            field_mask_t fieldBit = iterMatcher.key();
            QString const content = data.getFieldValue(fieldBit).toString();
            QSet<QString> const& textsForMatching = iterMatcher.value();
            for (QString const& textForMatching : textsForMatching) {
                if (content.contains(textForMatching)) {
                    pass = true;
                    break;
                }
            }
        }

        if (pass) {
            pidSet.insert(pid);
        }
    }
    return FilterError::NONE;
}

FilterError FrontendProcessFilter::doUpdateArguments(QString& error) {
    error.clear();
    return FilterError::NONE;
}

void FrontendProcessFilter::doMerge(IProcessFilter const& otherBase) {
    FrontendProcessFilter const& other
        = dynamic_cast<FrontendProcessFilter const&>(otherBase);
    
    for (auto iter = other.m_textMatchers.cbegin(); iter != other.m_textMatchers.cend(); ++iter) {
        field_mask_t fieldBit = iter.key();
        m_textMatchers[fieldBit].unite(iter.value());
    }
}

FilterError FrontendProcessFilter::doDigestArguments(
    QList<QList<QVariant>> const& argsList
) {
    for (QList<QVariant> const& args : argsList) {
        Q_ASSERT(args.size() >= 2);
        Q_ASSERT(args[0].canConvert<field_mask_t>());
        Q_ASSERT(args[1].canConvert<QString>());

        field_mask_t fieldBit = args[0].value<field_mask_t>();
        QString textToMatch = args[1].toString();
        m_textMatchers[fieldBit].insert(textToMatch);
    }
    return FilterError::NONE;
}
