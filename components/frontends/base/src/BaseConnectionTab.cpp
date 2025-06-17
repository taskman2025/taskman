#include "taskman/frontends/BaseConnectionTab.h"
#include "taskman/frontends/FrontendProcessFilter.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTreeView>
#include <QVBoxLayout>

BaseConnectionTab::BaseConnectionTab(IConnection* connection, QWidget* parent)
    : QWidget(parent),
      m_connection{connection},
      m_model{new BaseProcessItemModel(connection, this)},
      m_treeView{nullptr} {
}

BaseConnectionTab::BaseConnectionTab(BaseProcessItemModel* model, QWidget* parent)
    : QWidget(parent),
      m_connection{model->getConnection()},
      m_model{model},
      m_treeView{nullptr} {
}

BaseConnectionTab::~BaseConnectionTab() {
    delete m_model;
    delete m_connection;
}

IConnection* BaseConnectionTab::getConnection() const {
    return m_connection;
}

BaseProcessItemModel* BaseConnectionTab::getProcessItemModel() const {
    return m_model;
}

void BaseConnectionTab::buildUi() {
    m_treeView = new QTreeView(this);
    m_treeView->setSortingEnabled(true);
    m_treeView->setColumnWidth(0, 500);
    m_treeView->header()->setSectionResizeMode(QHeaderView::Interactive);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_logArea = new QTextEdit(this);

    m_proxyModel = createProxyModel(this);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setSourceModel(m_model);
    m_treeView->setModel(m_proxyModel);

    m_statusBar = new QStatusBar(this);
    m_statusBar->showMessage("Connecting...");

    QVBoxLayout* layout = new QVBoxLayout;
    {
        QSplitter* splitter = new QSplitter(Qt::Vertical, this);
        {
            QWidget* treeViewAndRelatedControls = new QWidget(this);
            {
                QVBoxLayout* layout = new QVBoxLayout;
                layout->addWidget(m_treeView);
                {
                    QWidget* actionsContainer = new QWidget(this);
                    {
                        QHBoxLayout* layout = new QHBoxLayout;
                        // Add a spacer to push buttons to the right
                        layout->addStretch();
                        for (ProcessAction const& action : m_connection->getPlatformProfile().getProcessActions()) {
                            QPushButton* button = new QPushButton(action.name, this);
                            process_action_id_t actionId = action.id;
                            connect(button, &QPushButton::clicked, this, [this, actionId]() {
                                performActionOnTheCurrentlySelectedProcess(actionId);
                            });
                            layout->addWidget(button);
                        }
                        actionsContainer->setLayout(layout);
                    }
                    layout->addWidget(actionsContainer);
                }
                treeViewAndRelatedControls->setLayout(layout);
            }
            splitter->addWidget(treeViewAndRelatedControls);
        }
        splitter->addWidget(m_logArea);
        splitter->setStretchFactor(0, 10);
        splitter->setStretchFactor(1, 1);
        layout->addWidget(splitter);
    }
    layout->addWidget(m_statusBar);
    setLayout(layout);

    m_filterPopup = createFilterPopup(this);
}

void BaseConnectionTab::initialize() {
    buildUi();

    connect(
        m_connection,
        &IConnection::initialized,
        this,
        [this]() {
            appendLog("Connection initialized", Qt::black);
        }
    );

    connect(
        m_connection,
        &IConnection::initializationError,
        this,
        [this](ConnectionError e) {
            appendLog(QString("Connection initialization failed. Please reopen this tab.\n") + e.manifest(), Qt::red);
        }
    );

    connect(
        m_connection,
        &IConnection::connectionInitiated,
        this,
        [this](CommunicationPractices practices) {
            appendLog("Connection initiated", Qt::green);
        }
    );

    connect(
        m_connection, &IConnection::connectionInitiationError, this, [this](ConnectionError e) {
            appendLog(QString("Connection could not be established. Please reopen this tab.\n") + e.manifest(), Qt::red);
        }
    );

    connect(m_connection, &IConnection::replyProcessTree, this, &BaseConnectionTab::onReplyProcessTree);
    connect(m_connection, &IConnection::replyProcessTreeError, this, &BaseConnectionTab::onReplyProcessTreeError);
    connect(m_connection, &IConnection::replySystemInformation, this, &BaseConnectionTab::onReplySystemInformation);
    connect(m_connection, &IConnection::replyProcessAction, this, &BaseConnectionTab::onReplyProcessAction);
    connect(m_connection, &IConnection::replyProcessActionError, this, &BaseConnectionTab::onReplyProcessActionError);
    connect(m_connection, &IConnection::replyProcessFiltering, this, &BaseConnectionTab::onReplyProcessFiltering);
    connect(m_connection, &IConnection::replyProcessFilteringError, this, &BaseConnectionTab::onReplyProcessFilteringError);
    connect(m_connection, &IConnection::refilterProcessesNow, this, &BaseConnectionTab::onRefilterProcessNow);
    connect(this, &BaseConnectionTab::requestProcessFiltering, m_connection, &IConnection::requestProcessFiltering);
    connect(this, &BaseConnectionTab::requestProcessAction, m_connection, &IConnection::requestProcessAction);

    m_model->initialize();
    m_connection->initiateConnection();
}

#define MAX_CHARS_IN_LOG 10'000
void BaseConnectionTab::appendLog(QString const& message, QColor const& color) {
    QTextCharFormat format;
    format.setForeground(color);

    QTextCursor cursor = m_logArea->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(message + '\n', format);
    m_logArea->setTextCursor(cursor);

    if (m_logArea->toPlainText().length() > MAX_CHARS_IN_LOG) {
        QTextCursor trimCursor(m_logArea->document());
        trimCursor.movePosition(QTextCursor::Start);
        trimCursor.movePosition(
            QTextCursor::NextCharacter,
            QTextCursor::KeepAnchor,
            m_logArea->toPlainText().length() - MAX_CHARS_IN_LOG
        );
        trimCursor.removeSelectedText();
    }
}
#undef MAX_CHARS_IN_LOG

BaseSortFilterProxyModel* BaseConnectionTab::createProxyModel(QObject* parent) {
    return new BaseSortFilterProxyModel(this, parent);
}

void BaseConnectionTab::performActionOnTheCurrentlySelectedProcess(process_action_id_t actionId) {
    QModelIndex index = m_treeView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return;
    }

    pid_t pid = BaseProcessItemModel::anyIndexToPid(
        index,
        m_connection->getPlatformProfile().getImaginaryRootProcId()
    );

    for (ProcessAction const& action : m_connection->getPlatformProfile().getProcessActions()) {
        if (action.id == actionId) {
            if (!action.confirmationMessage.isEmpty()) {
                QString confirmText = action.confirmationMessage;
                if (!action.confirmationMessageArguments.isEmpty()) {
                    QVariant procRawData = m_model->data(index, Qt::UserRole);
                    if (!procRawData.isValid() || !procRawData.canConvert<ProcessData>()) {
                        qWarning() << "proc raw data is invalid";
                        return;
                    }
                    ProcessData data = procRawData.value<ProcessData>();
                    for (field_mask_t const fieldArg : action.confirmationMessageArguments) {
                        confirmText = confirmText.arg(
                            data.getFieldValue(fieldArg).toString()
                        );
                    }
                }
                if (QMessageBox::Ok != QMessageBox::question(this, "Confirm", confirmText, QMessageBox::Ok | QMessageBox::Cancel)) {
                    return;
                }
            }
            emit requestProcessAction(actionId, pid);
            return;
        }
    }
    throw std::invalid_argument(QString("No such action id: %1").arg(actionId).toStdString());
}

void BaseConnectionTab::onReplyProcessTree(ThreadsafeSharedConstPointer<ProcessTree> newTree) {
    if (newTree.get() == nullptr) {
        return;
    }
    m_numProcs = newTree->totalNumProcs;
    m_lastTreeTimestamp = newTree->timestamp;
    updateStatusBar();
}
void BaseConnectionTab::onReplyProcessTreeError(ConnectionError e) {
    appendLog(QString("Error while updating process tree:\n") + e.manifest(), Qt::red);
}

void BaseConnectionTab::onReplySystemInformation(SystemInformation s) {
    m_systemInformation = s;
    updateStatusBar();
}
void BaseConnectionTab::onReplySystemInformationError(ConnectionError e) {
    appendLog(QString("Error while updating system information:\n") + e.manifest(), Qt::red);
}

void BaseConnectionTab::onReplyProcessAction(ActionResult result) {
    appendLog(result.message, (result.errorCode == ActionErrorCode::NONE ? Qt::green : Qt::red));
}
void BaseConnectionTab::onReplyProcessActionError(ConnectionError e) {
    appendLog(QString("Error while applying action to process:\n") + e.manifest(), Qt::red);
}

void BaseConnectionTab::onReplyProcessFiltering(
    QMap<filter_type_id_t, QPair<FilterError, QString>> errors,
    QSet<proc_id_t> filteredPidSet
) {
    m_proxyModel->updateFilters(filteredPidSet);
}
void BaseConnectionTab::onReplyProcessFilteringError(ConnectionError e) {
    appendLog(QString("Error while applying filters:\n") + e.manifest(), Qt::red);
}
void BaseConnectionTab::onRefilterProcessNow() {
    applyFilters(m_filterPopup->getFilterArgsList());
}
void BaseConnectionTab::applyFilters(QHash<filter_type_id_t, QList<QList<QVariant>>> filters) {
    QSet<proc_id_t> pidSet;
    if (filters.contains(FrontendProcessFilter::FILTER_TYPE.id)) {
        QList<QList<QVariant>> frontendFilterArgsList = filters.value(FrontendProcessFilter::FILTER_TYPE.id);
        FrontendProcessFilter filter(frontendFilterArgsList, m_model->snapshot());
        QString error;
        FilterError e = filter.apply(pidSet, error);
        if (e != FilterError::NONE) {
            appendLog(QString("Error while filtering processes (locally): ") + error, Qt::red);
            return;
        }
        filters.detach();
        filters.remove(FrontendProcessFilter::FILTER_TYPE.id);
    } else {
        auto map = m_model->snapshot();
        pidSet = QSet(map.keyBegin(), map.keyEnd());
    }

    emit requestProcessFiltering(pidSet, filters);
}

void BaseConnectionTab::updateStatusBar() {
    QString text = QString("%1 - %2 [%3 - %4] | %5 processes | Last updated %6")
                       .arg(m_systemInformation.osName)
                       .arg(m_systemInformation.osVersion)
                       .arg(m_systemInformation.osKernelType)
                       .arg(m_systemInformation.osKernelVersion)
                       .arg(m_numProcs)
                       .arg((m_lastTreeTimestamp > 0 ? QDateTime::fromSecsSinceEpoch(static_cast<qint64>(m_lastTreeTimestamp)) : QDateTime::currentDateTime()).toString("dd MMM yyyy - HH:mm:ss"));

    m_statusBar->showMessage(text);
}

BaseFilterPopup* BaseConnectionTab::createFilterPopup(QWidget* parent) {
    return new BaseFilterPopup(m_connection->getPlatformProfile(), parent);
}
