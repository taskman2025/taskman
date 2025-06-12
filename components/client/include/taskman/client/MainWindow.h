#ifndef MainWindow_INCLUDED
#define MainWindow_INCLUDED

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void setupClientArea();
    void setupMenu();

    QTabWidget* m_connectionTabs;
};
#endif // MainWindow_INCLUDED
