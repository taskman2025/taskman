#ifndef IConnectionTab_INCLUDED
#define IConnectionTab_INCLUDED

#include "taskman/common/types.h"
#include "taskman/backend/IProcessItemModel.h"
#include "taskman/backend/ISortFilterProxyModel.h"
#include <QSharedPointer>
#include <QStatusBar>
#include <QTimer>
#include <QTreeView>
#include <QWidget>
#include <QLayout>
#include <QPushButton>

class IBackend;

class IConnectionTab : public QWidget {
    Q_OBJECT

public:
    IConnectionTab(IBackend* backend);
    ~IConnectionTab();
    virtual ISortFilterProxyModel* createSortFilterProxyModel();

public slots:
    void updateNumProcs(proc_count_t numProcs);
    void onEndClicked();
    void onKillClicked();

protected:
    QLayout* m_extraComponentsLayout;
    QSharedPointer<IProcessItemModel> m_procItemModel;
    IBackend* const m_backend;
    QTreeView* m_treeView;
    QTimer* m_updateTimer;
    QStatusBar* m_statusBar;
    QPushButton* m_endButton;
    QPushButton* m_killButton;
};

#endif // IConnectionTab_INCLUDED
