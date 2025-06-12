#ifndef ISortFilterProxyModel_INCLUDED
#define ISortFilterProxyModel_INCLUDED

#include <QObject>
#include <QSortFilterProxyModel>

class ISortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    bool doFilterAcceptsRow(int source_row, const QModelIndex& source_parent, int depth = 6, int take = 10) const;
};

#endif // ISortFilterProxyModel_INCLUDED
