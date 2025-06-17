#include "taskman/frontends/BaseFilterPopup.h"
#include "taskman/common/FilterParamType.h"
#include "taskman/common_ui/FileInputWidget.h"
#include "taskman/frontends/FrontendProcessFilter.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

void BaseFilterPopup::createFilterSection(ProcessFilterType const& type) {
    QWidget* filterSection = new QWidget(this);
    QList<QVariant>* filterSectionData = new QList<QVariant>(type.parameters.size());
    filter_type_id_t filterTypeId = type.id;
    m_filterData[filterTypeId].insert(filterSectionData);

    QVBoxLayout* layout = new QVBoxLayout;
    {
        QWidget* captionPanel = new QWidget(filterSection);
        {
            QHBoxLayout* layout = new QHBoxLayout;
            {
                QLabel* captionLabel = new QLabel(type.name, filterSection);
                layout->addWidget(captionLabel, 10);
            }
            layout->addStretch();
            {
                QPushButton* closeButton = new QPushButton("X", filterSection);
                connect(closeButton, &QPushButton::clicked, [this, filterSection, filterSectionData, filterTypeId]() {
                    filterSection->close();
                    filterSection->deleteLater();
                    m_filterData[filterTypeId].remove(filterSectionData);
                    delete filterSectionData;
                });
                filterSection->setAttribute(Qt::WA_DeleteOnClose);
                layout->addWidget(closeButton, 0);
            }
            captionPanel->setLayout(layout);
        }
        layout->addWidget(captionPanel);
    }

    {
        QLabel* description = new QLabel(type.description, filterSection);
        description->setWordWrap(true);
        layout->addWidget(description);
    }

    {
        int i = 0;
        for (filter_param_type_mask_t filterParamType : type.parameters) {
            switch (filterParamType) {
            case FilterParamType::TEXT: {
                (*filterSectionData)[i] = "";
                QLineEdit* input = new QLineEdit(filterSection);
                connect(input, &QLineEdit::textChanged, this, [this, filterSectionData, i](QString const& newText) {
                    (*filterSectionData)[i] = newText;
                });
                layout->addWidget(input);
                adjustSize();
            } break;

            case FilterParamType::PROCESS_FIELD: {
                (*filterSectionData)[i] = QVariant::fromValue(m_platformProfile.getProcessFields()[0].mask); // since by default QComboBox selects the first option
                QComboBox* dropdown = new QComboBox(filterSection);
                // TODO: get selective fields from connection tab
                for (ProcessField const& field : m_platformProfile.getProcessFields()) {
                    dropdown->addItem(field.name, QVariant::fromValue(field.mask));
                }
                connect(dropdown, &QComboBox::currentIndexChanged, this, [this, filterSectionData, dropdown, i](int index) {
                    QVariant data = dropdown->itemData(index);
                    if (!data.isValid()) {
                        qDebug() << "No valid option selected.";
                        return;
                    }
                    (*filterSectionData)[i] = data;
                });
                layout->addWidget(dropdown);
                adjustSize();
            } break;

            case FilterParamType::EXISTING_FILE_PATH: {
                // TODO: different logic for remote connections!
                (*filterSectionData)[i] = "";
                FileInputWidget* fileInput = new FileInputWidget(filterSection, this);
                connect(fileInput, &FileInputWidget::filePathChanged, this, [this, filterSectionData, i](QString filePath) {
                    (*filterSectionData)[i] = filePath;
                });
                layout->addWidget(fileInput);
                adjustSize();

            } break;

            default:
                break;
            }
            ++i;
        }
    }

    filterSection->setLayout(layout);
    m_filterArea->addWidget(filterSection);
}

BaseFilterPopup::BaseFilterPopup(IPlatformProfile const& platformProfile, QWidget* parent)
    : QWidget(parent), m_platformProfile{platformProfile} {
}

void BaseFilterPopup::initialize() {
    hide();
    setWindowFlags(Qt::Window);
    setWindowTitle("Filters");

    QMenu* filterMenu = new QMenu(this);

    filterMenu->addAction(
        FrontendProcessFilter::FILTER_TYPE.name, this, [this]() { createFilterSection(FrontendProcessFilter::FILTER_TYPE); }
    );
    for (ProcessFilterType const& filterType : m_platformProfile.getProcessFilterTypes()) {
        filterMenu->addAction(
            filterType.name, this, [this, &filterType]() { createFilterSection(filterType); }
        );
    }

    QPushButton* createFilterButton = new QPushButton("Add filter", this);
    connect(createFilterButton, &QPushButton::clicked, this, [filterMenu, createFilterButton]() {
        QPoint pos = createFilterButton->mapToGlobal(QPoint(0, createFilterButton->height()));
        filterMenu->exec(pos); // Show menu just below the button
    });

    QPushButton* applyButton = new QPushButton("Apply current filters", this);
    connect(applyButton, &QPushButton::clicked, this, &BaseFilterPopup::onApplyButtonClicked);

    QVBoxLayout* layout = new QVBoxLayout;

    {
        QWidget* filtersArea = new QWidget(this);
        {
            m_filterArea = new QVBoxLayout;
            filtersArea->setLayout(m_filterArea);
        }
        layout->addWidget(filtersArea);
    }
    {
        QWidget* buttonsContainer = new QWidget(this);
        {
            QHBoxLayout* buttonsLayout = new QHBoxLayout;
            buttonsLayout->addStretch();
            buttonsLayout->addWidget(createFilterButton);
            buttonsLayout->addWidget(applyButton);
            buttonsLayout->addStretch();
            buttonsContainer->setLayout(buttonsLayout);
        }
        layout->addWidget(buttonsContainer);
    }

    setLayout(layout);
}

void BaseFilterPopup::onApplyButtonClicked() {
    emit updateFilters(getFilterArgsList());
}

QHash<filter_type_id_t, QList<QList<QVariant>>> BaseFilterPopup::getFilterArgsList() {
    QHash<filter_type_id_t, QList<QList<QVariant>>> filterArgsList;

    for (auto iter = m_filterData.cbegin(); iter != m_filterData.cend(); ++iter) {
        filter_type_id_t filterTypeId = iter.key();
        for (QList<QVariant>* listPtr : iter.value()) {
            // TODO: detach?
            // TODO: validate input?
            filterArgsList[filterTypeId].append(*listPtr);
        }
    }

    return filterArgsList;
}
