#ifndef BaseConnectionTab_INCLUDED
#define BaseConnectionTab_INCLUDED

#include "taskman/connections/IConnection.h"
#include "taskman/frontends/BaseProcessItemModel.h"
#include "taskman/frontends/BaseSortFilterProxyModel.h"
#include "taskman/frontends/BaseFilterPopup.h"
#include <QWidget>
#include <QTextEdit>
#include <QColor>
#include <QTreeView>
#include <QStatusBar>

class BaseConnectionTab : public QWidget {
    Q_OBJECT

public:
    /**
     * Instances of this class take ownership of the connection handle.
     */
    BaseConnectionTab(IConnection* connection, QWidget* parent = nullptr);

    /**
     * Instances of this class take ownership of the process item model pointer.
     */
    BaseConnectionTab(BaseProcessItemModel* model, QWidget* parent = nullptr);

    virtual ~BaseConnectionTab();
    IConnection* getConnection() const;
    BaseProcessItemModel* getProcessItemModel() const;

signals:
    void requestProcessAction(
        process_action_id_t actionId,
        proc_id_t pid
    );
signals:
    void requestProcessFiltering(
        QSet<proc_id_t> pidSet,
        QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList
    );

public slots:
    void initialize();

protected:
    virtual BaseSortFilterProxyModel* createProxyModel(QObject* parent);
    virtual BaseFilterPopup* createFilterPopup(QWidget* parent);

private:
    IConnection* const m_connection;
    BaseProcessItemModel* const m_model;
    BaseSortFilterProxyModel* m_proxyModel;
    BaseFilterPopup* m_filterPopup;
    QTreeView* m_treeView;
    QTextEdit* m_logArea;
    QStatusBar* m_statusBar;

    SystemInformation m_systemInformation;
    proc_count_t m_numProcs;
    timestamp_t m_lastTreeTimestamp{0};

    void buildUi();
    void performActionOnTheCurrentlySelectedProcess(process_action_id_t actionId);
    void appendLog(QString const& message, QColor const& color);
    void updateStatusBar();

private slots:
    void onReplyProcessTree(ThreadsafeSharedConstPointer<ProcessTree> newTree);
    void onReplyProcessTreeError(ConnectionError e);
    void onReplySystemInformation(SystemInformation s);
    void onReplySystemInformationError(ConnectionError e);
    void onReplyProcessAction(ActionResult result);
    void onReplyProcessActionError(ConnectionError e);
    void onReplyProcessFiltering(
        QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
        QSet<proc_id_t> filteredPidSet
    );
    void onReplyProcessFilteringError(ConnectionError e);
    void onRefilterProcessNow();
    void applyFilters(QHash<filter_type_id_t, QList<QList<QVariant>>> filters);
};

#endif // BaseConnectionTab_INCLUDED
