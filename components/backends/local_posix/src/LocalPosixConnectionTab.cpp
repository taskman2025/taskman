#include "taskman/backend/local_posix/LocalPosixConnectionTab.h"
#include "taskman/backend/IBackend.h"
#include "taskman/backend/local_posix/PosixProcessItemModel.h"
#include "taskman/platform/PosixProcessFilter.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LocalPosixConnectionTab::LocalPosixConnectionTab(IBackend* backend)
    : IConnectionTab(backend) {
    QWidget* widget = new QWidget(this);
    QLayout* vLayout = new QVBoxLayout(widget);

    QLayout* hLayout = new QHBoxLayout(widget);
    fileInput = new FileInputWidget(
        [this](QString filePath) {
            // PosixProcessItemModel* model = dynamic_cast<PosixProcessItemModel*>(m_procItemModel.data());
            m_procItemModel->clearFilter(); // TODO: clear filter with type OPEN_FILE
            if (!filePath.isEmpty()) {
                m_procItemModel->addFilter(
                    ThreadsafeConstSharedPointer<IProcessFilter>{
                        m_backend->getPlatform()->createProcessFilter(
                            FilterBit::OPEN_FILE, filePath
                        )
                    }
                );
            }
        },
        this
    );
    fileInput->setPlaceholderText("Filter processes that are opening a file");
    hLayout->addWidget(fileInput);
    vLayout->addItem(hLayout);

    widget->setLayout(vLayout);

    m_extraComponentsLayout->addWidget(widget);

    qDebug() << "this";
}
