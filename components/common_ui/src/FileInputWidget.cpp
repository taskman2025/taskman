#include "taskman/common_ui/FileInputWidget.h"

FileInputWidget::FileInputWidget(std::function<void(const QString&)> onFileSelected, QWidget* parent)
    : QWidget(parent), callback(std::move(onFileSelected)) {
    filePathEdit = new QLineEdit(this);
    browseButton = new QPushButton("...", this);
    submitButton = new QPushButton("Submit", this);

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(filePathEdit);
    hLayout->addWidget(browseButton);
    hLayout->addWidget(submitButton);

    setLayout(hLayout);

    connect(browseButton, &QPushButton::clicked, this, &FileInputWidget::onBrowseClicked);
    connect(submitButton, &QPushButton::clicked, this, &FileInputWidget::onSubmitClicked);
}

void FileInputWidget::setPlaceholderText(QString placeholder) {
    filePathEdit->setPlaceholderText(placeholder);
}

void FileInputWidget::onBrowseClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select a file");
    if (!filePath.isEmpty()) {
        filePathEdit->setText(filePath);
    }
}

void FileInputWidget::onSubmitClicked() {
    QString path = filePathEdit->text();
    if (!path.isEmpty()) {
        QFileInfo info(path);
        if (!info.exists() || !info.isFile()) {
            QMessageBox::warning(this, "Invalid File", "The selected file does not exist.");
            return;
        }
    }
    callback(path);
}
