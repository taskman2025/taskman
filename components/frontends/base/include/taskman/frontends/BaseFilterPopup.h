#ifndef BaseFilterPopup_INCLUDED
#define BaseFilterPopup_INCLUDED

#include <QWidget>
#include <QWindow>
#include <QHash>
#include <QSet>
#include <QVBoxLayout>
#include "taskman/common/types.h"
#include "taskman/platform_profiles/ProcessFilterType.h"
#include "taskman/platform_profiles/IPlatformProfile.h"

class BaseFilterPopup : public QWidget {
    Q_OBJECT

public:
    BaseFilterPopup(IPlatformProfile const& platformProfile, QWidget* parent = nullptr);
    void initialize();
    QHash<filter_type_id_t, QList<QList<QVariant>>> getFilterArgsList();

signals:
    void updateFilters(QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList);

private slots:
    void onApplyButtonClicked();
    
private:
    void createFilterSection(ProcessFilterType const& type);
    QHash<filter_type_id_t, QSet<QList<QVariant>*>> m_filterData;
    IPlatformProfile const& m_platformProfile;
    QVBoxLayout* m_filterArea;
};

#endif // BaseFilterPopup_INCLUDED
