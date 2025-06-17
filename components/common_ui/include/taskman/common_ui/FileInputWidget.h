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
    FileInputWidget(QWidget* parent, QWidget* parentForPopup);

signals:
    void filePathChanged(QString filePath);

public slots:
    void setPlaceholderText(QString text);

private slots:
    void onBrowseClicked();
    void onFilePathChanged(QString filePath);

private:
    QLineEdit* filePathEdit;
    QPushButton* browseButton;
    QWidget* const m_parentForPopup;
};

#endif // FileInputWidget_INCLUDED
