#include "taskman/client/MainWindow.h"
#include "taskman/frontends/BaseConnectionTab.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QCoreApplication>

BaseConnectionTab* createLocalConnectionTab(QWidget* parent);

// TODO: depends on platform
// TODO: how about BSDs?
#if defined(__linux__) && !defined(__APPLE__)
#include "taskman/connections/LocalPosixConnection.h"
#include "taskman/frontends/PosixProcessItemModel.h"

BaseConnectionTab* createLocalConnectionTab(QWidget* parent) {
    IConnection* connection = new LocalPosixConnection();
    BaseProcessItemModel* model = new PosixProcessItemModel(connection);
    return new BaseConnectionTab(model, parent, parent);
}

#else
#error Platform not yet supported
#endif

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupClientArea();
    setupMenu();
}

MainWindow::~MainWindow() {}

void MainWindow::setupClientArea() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout;

    m_connectionTabsContainer = new QTabWidget(this);
    BaseConnectionTab* tab1 = createLocalConnectionTab(this);
    tab1->initialize();
    m_connectionTabsContainer->addTab(tab1, "Local");

    setCentralWidget(m_connectionTabsContainer);
    setWindowTitle(QString("taskman (PID = %1)").arg(QCoreApplication::applicationPid()));
    resize(800, 600);
}

void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    QAction* exitAction = fileMenu->addAction("Quit");

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QAction* aboutAction = helpMenu->addAction("&About");

    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About", u8""
                                          "taskman for Linux, version 1.0.1\n"
                                          "Copyright (c) 2025 Vũ Tùng Lâm\n"
                                          "Distributed under GPLv3");
    });
}
