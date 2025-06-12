#ifndef FileInputWidget_INCLUDED
#define FileInputWidget_INCLUDED

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <functional>

class FileInputWidget : public QWidget {
    Q_OBJECT

public:
    FileInputWidget(std::function<void(const QString&)> onFileSelected, QWidget* parent = nullptr);

public slots:
    void setPlaceholderText(QString text);

private slots:
    void onBrowseClicked();

    void onSubmitClicked();

private:
    QLineEdit* filePathEdit;
    QPushButton* browseButton;
    QPushButton* submitButton;
    std::function<void(const QString&)> callback;
};

#endif // FileInputWidget_INCLUDED
