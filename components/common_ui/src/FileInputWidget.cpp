#include "taskman/common_ui/FileInputWidget.h"

FileInputWidget::FileInputWidget(QWidget* parent, QWidget* parentForPopup)
    : QWidget(parent), m_parentForPopup{parentForPopup} {
    filePathEdit = new QLineEdit(this);
    browseButton = new QPushButton("...", this);

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(filePathEdit, 10);
    hLayout->addWidget(browseButton, 0);

    setLayout(hLayout);

    connect(browseButton, &QPushButton::clicked, this, &FileInputWidget::onBrowseClicked);
    connect(filePathEdit, &QLineEdit::textChanged, this, &FileInputWidget::onFilePathChanged);
}

void FileInputWidget::setPlaceholderText(QString placeholder) {
    filePathEdit->setPlaceholderText(placeholder);
}

void FileInputWidget::onBrowseClicked() {
    QString filePath = QFileDialog::getOpenFileName(m_parentForPopup, "Select a file");
    if (!filePath.isEmpty()) {
        filePathEdit->setText(filePath);
    }
}

void FileInputWidget::onFilePathChanged(QString filePath) {
    emit filePathChanged(filePath);
}
