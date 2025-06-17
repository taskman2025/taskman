#ifndef MainWindow_INCLUDED
#define MainWindow_INCLUDED

#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void setupClientArea();
    void setupMenu();

    QTabWidget* m_connectionTabsContainer;
};
#endif // MainWindow_INCLUDED
