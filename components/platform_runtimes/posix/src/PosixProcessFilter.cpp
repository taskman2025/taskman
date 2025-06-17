#include "taskman/platform_runtimes/posix/PosixProcessFilter.h"
#include "taskman/platform_profiles/posix/process_filter_type_ids.h"
#include "taskman/platform_runtimes/posix/process_filters/PosixOpenFileProcessFilter.h"
#include <stdexcept>

PosixBaseProcessFilter* PosixProcessFilter::createInternalFilter(
    filter_type_id_t filterTypeId,
    QList<QList<QVariant>> const& argsList
) {
    switch (filterTypeId) {
    case PosixProcessFilterTypeID::OPEN_FILE:
        return new PosixOpenFileProcessFilter(argsList);
    default:
        throw std::invalid_argument(std::string("bad filter type id: ") + std::to_string(filterTypeId));
    }
}
