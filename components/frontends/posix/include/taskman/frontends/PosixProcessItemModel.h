#ifndef PosixProcessItemModel_INCLUDED
#define PosixProcessItemModel_INCLUDED

#include "taskman/frontends/BaseProcessItemModel.h"

class PosixProcessItemModel : public BaseProcessItemModel {
    Q_OBJECT

public:
    using BaseProcessItemModel::BaseProcessItemModel;

protected:
    virtual QVariant customData(
        QModelIndex const& index,
        int role,
        proc_id_t pid,
        ProcessData const& processData
    ) const override;
};

#endif // PosixProcessItemModel_INCLUDED
