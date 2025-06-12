#ifndef PosixProcessItemModel_INCLUDED
#define PosixProcessItemModel_INCLUDED

#include "taskman/backend/IProcessItemModel.h"
#include "taskman/backend/ProcessData.h"
#include "taskman/platform/PosixPlatform.h"
#include <QList>

class PosixProcessItemModel : public IProcessItemModel {
    Q_OBJECT
public:
    PosixProcessItemModel(PosixPlatform& platform, QObject* parent = nullptr);

    virtual ~PosixProcessItemModel() override;

    virtual proc_id_t getImaginaryRootProcId() const override;

    virtual void validateIds(proc_id_t& pid, proc_id_t& ppid, bool& ok) const override;

    virtual QVariant customData(QModelIndex const& index, int role, proc_id_t pid, ProcessData const& processData) const override;
};

#endif // PosixProcessItemModel_INCLUDED
