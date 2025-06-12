#include "taskman/backend/ISortFilterProxyModel.h"

bool ISortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    return doFilterAcceptsRow(source_row, source_parent);
}

bool ISortFilterProxyModel::doFilterAcceptsRow(int source_row, const QModelIndex& source_parent, int depth, int take) const {
    if (depth < 0) return false;
    // return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    int numCols = sourceModel()->columnCount();

    // TODO: allow PID, process name and command line search!
    // Maybe in IProcessItemModel.h
    // Now: hardcode
    for (int col : QList<int>{0, 1, numCols - 2}) {
        QModelIndex index = sourceModel()->index(source_row, col, source_parent);
        // Direct match
        QVariant value = sourceModel()->data(index);
        if (value.canConvert<QString>()) {
            if (value.toString().contains(filterRegularExpression()))
                return true;

            // Recursive check: any child matches?
            const int rowCount = std::max(sourceModel()->rowCount(index), take);
            for (int i = 0; i < rowCount; ++i) {
                if (doFilterAcceptsRow(i, index, depth - 1, take))
                    return true;
            }
        }
    }
    // No match
    return false;
}
