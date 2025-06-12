#include "taskman/client/MainWindow.h"
#include "taskman/backend/IConnectionTab.h"
#include "taskman/backend/local_posix/LocalPosixBackend.h"
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTabWidget>
#include <QTreeView>
#include <QVBoxLayout>

QList<IBackend*> g_backends = {
    new LocalPosixBackend()
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupClientArea();
    setupMenu();
}

MainWindow::~MainWindow() {}

void MainWindow::setupClientArea() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout;

    m_connectionTabs = new QTabWidget(this);
    IConnectionTab* tab1 = g_backends[0]->createConnectionTab();
    m_connectionTabs->addTab(tab1, "Local");

    setCentralWidget(m_connectionTabs);
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
                                          "taskman for Linux, version 0.1\n"
                                          "Copyright (c) 2025 Vũ Tùng Lâm\n"
                                          "Distributed under GPLv3");
    });
}
