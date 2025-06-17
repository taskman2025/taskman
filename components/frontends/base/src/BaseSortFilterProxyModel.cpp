#include "taskman/frontends/BaseSortFilterProxyModel.h"
#include "taskman/frontends/BaseConnectionTab.h"
#include "taskman/frontends/BaseProcessItemModel.h"

BaseSortFilterProxyModel::BaseSortFilterProxyModel(
    BaseConnectionTab* connectionTab,
    QObject* parent
)
    : QSortFilterProxyModel(parent),
      m_filteredPidSetProxy{},
      m_imaginaryRootPid{connectionTab->getConnection()->getPlatformProfile().getImaginaryRootProcId()} {
}

BaseSortFilterProxyModel::~BaseSortFilterProxyModel() = default;

void BaseSortFilterProxyModel::updateFilters(QSet<proc_id_t> const set) {
    QSet<proc_id_t>* setPtr = new QSet<proc_id_t>{set};
    setPtr->detach();
    m_filteredPidSetProxy.set(setPtr);
    invalidateFilter();
}

bool BaseSortFilterProxyModel::filterAcceptsRow(
    int source_row,
    QModelIndex const& source_parent
) const {
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    proc_id_t pid = BaseProcessItemModel::anyIndexToPid(
        index,
        m_imaginaryRootPid
    );
    ThreadsafeSharedConstPointer<QSet<proc_id_t>> filteredPidSetPtr{
        m_filteredPidSetProxy.get()
    };
    // reduce locking overhead: passing raw pointer
    // to the following synchronous function - at the end,
    // that function returns, then filterAcceptRows()
    // returns and the filteredPidSetPtr gets popped off
    // the stack, i.e. released automatically
    return hasMatchingDescendant(
        *filteredPidSetPtr.get(),
        index
    );
}

bool BaseSortFilterProxyModel::hasMatchingDescendant(
    QSet<proc_id_t> const& filteredPidSet,
    QModelIndex const& parent
) const {
    int const numRows = sourceModel()->rowCount();
    if (numRows == 0) {
        return false;
    }

    for (int iRow = 0; iRow < numRows; ++iRow) {
        QModelIndex childIndex = sourceModel()->index(iRow, 0, parent);
        proc_id_t childPid = BaseProcessItemModel::anyIndexToPid(childIndex, m_imaginaryRootPid);
        if (filteredPidSet.contains(childPid)) {
            return true;
        }

        if (hasMatchingDescendant(filteredPidSet, childIndex)) {
            return true;
        }
    }
    return false;
}
