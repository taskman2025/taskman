#ifndef BaseSortFilterProxyModel_INCLUDED
#define BaseSortFilterProxyModel_INCLUDED

#include "taskman/common/types.h"
#include "taskman/common/ThreadsafeConstReadProxy.h"
#include <QSortFilterProxyModel>

class BaseConnectionTab;

class BaseSortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    /**
     * Instances of this class do not take owner ship of the connection tab pointer.
     */
    BaseSortFilterProxyModel(
        BaseConnectionTab* connectionTab,
        QObject* parent = nullptr
    );
    virtual ~BaseSortFilterProxyModel();

public slots:
    void updateFilters(QSet<proc_id_t> const set);

protected:
    virtual bool filterAcceptsRow(int source_row, QModelIndex const& source_parent) const override;

private:
    virtual bool hasMatchingDescendant(
        QSet<proc_id_t> const& filteredPidSet,
        QModelIndex const& parent
    ) const;
    ThreadsafeConstReadProxy<QSet<proc_id_t>> m_filteredPidSetProxy;
    proc_id_t const m_imaginaryRootPid;
};

#endif // BaseSortFilterProxyModel_INCLUDED
