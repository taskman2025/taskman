#include "taskman/client/MainWindow.h"
#include "taskman/backends/ProcessData.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qRegisterMetaType<ProcessData>();
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
