#include "taskman/backend/IConnectionTab.h"
#include "taskman/backend/IBackend.h"
#include <QDateTime>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

IConnectionTab::IConnectionTab(IBackend* backend) : m_backend{backend} {
    QVBoxLayout* layout = new QVBoxLayout;
    m_extraComponentsLayout = new QVBoxLayout;

    m_treeView = new QTreeView(this);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->setColumnWidth(0, 300);
    m_treeView->header()->setSectionResizeMode(QHeaderView::Interactive);

    m_procItemModel.reset(backend->createProcessItemModel());

    ISortFilterProxyModel* proxyModel = createSortFilterProxyModel();
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setSourceModel(m_procItemModel.data());
    m_treeView->setModel(proxyModel);

    QLineEdit* searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Find by process name, command line, etc.");
    connect(searchInput, &QLineEdit::textChanged, proxyModel, &ISortFilterProxyModel::setFilterFixedString);

    m_statusBar = new QStatusBar(this);
    connect(m_procItemModel.data(), &IProcessItemModel::rebuildTreeComplete, this, &IConnectionTab::updateNumProcs);

    m_procItemModel->startRebuildingTree();
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, m_procItemModel.data(), &IProcessItemModel::startRebuildingTree);
    m_updateTimer->start(1000);

    layout->addItem(m_extraComponentsLayout);
    layout->addWidget(searchInput);
    layout->addWidget(m_treeView);

    QWidget* container = new QWidget(this);
    {
        QHBoxLayout* layout = new QHBoxLayout(container);

        // Add a spacer to push buttons to the right
        layout->addStretch();

        m_endButton = new QPushButton("End");
        m_killButton = new QPushButton("Kill");

        layout->addWidget(m_endButton);
        layout->addWidget(m_killButton);

        container->setLayout(layout);
    }
    layout->addWidget(container);

    layout->addWidget(m_statusBar);
    setLayout(layout);

    connect(m_endButton, &QPushButton::clicked, this, &IConnectionTab::onEndClicked);
    connect(m_killButton, &QPushButton::clicked, this, &IConnectionTab::onKillClicked);
}

IConnectionTab::~IConnectionTab() {
    m_updateTimer->stop();
}

void IConnectionTab::updateNumProcs(proc_count_t numProcs) {
    QSharedPointer<IPlatform> platform = m_backend->getPlatform();
    m_statusBar->showMessage(QString("%1 - %2 [%3] | %4 processes | Last updated %5").arg(platform->getOSName()).arg(platform->getOSVersion()).arg(platform->getPlatformName()).arg(numProcs).arg(QDateTime::currentDateTime().toString("dd MMM yyyy - HH:mm:ss")));
}

ISortFilterProxyModel* IConnectionTab::createSortFilterProxyModel() {
    return new ISortFilterProxyModel(this);
}

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

// TODO: Move this to platform or something

void IConnectionTab::onEndClicked() {
    QModelIndex index = m_treeView->currentIndex();
    pid_t pid = static_cast<pid_t>(index.data(Qt::UserRole).value<proc_id_t>());
    if (QMessageBox::Ok == QMessageBox::question(this, "Confirm", QString{"Are you sure to end process with PID = %1 ? (This will send SIGTERM.)"}.arg(pid), QMessageBox::Ok | QMessageBox::Cancel)) {
        int result = kill(pid, SIGTERM);
        if (result != 0) {
            if (errno == EPERM) {
                QMessageBox::critical(this, "Error", QString{"Not enough privilege. Please re-run the program with root/admin privilege, then try again."});
            } else if (errno == ESRCH) {
                QMessageBox::critical(this, "Error", QString{"No such process."});
            } else {
                QMessageBox::critical(this, "Error", QString("Could not terminate process. errno = %1").arg(errno));
            }
        }
    }
}

void IConnectionTab::onKillClicked() {
    QModelIndex index = m_treeView->currentIndex();
    pid_t pid = static_cast<pid_t>(index.data(Qt::UserRole).value<proc_id_t>());
    if (QMessageBox::Ok == QMessageBox::question(this, "Confirm", QString{"Are you sure to kill process with PID = %1 ? (This will send SIGKILL.)"}.arg(pid), QMessageBox::Ok | QMessageBox::Cancel)) {
        int result = kill(pid, SIGKILL);
        if (result != 0) {
            if (errno == EPERM) {
                QMessageBox::critical(this, "Error", QString{"Not enough privilege. Please re-run the program with root/admin privilege, then try again."});
            } else if (errno == ESRCH) {
                QMessageBox::critical(this, "Error", QString{"No such process."});
            } else {
                QMessageBox::critical(this, "Error", QString("Could not kill process. errno = %1").arg(errno));
            }
        }
    }
}
