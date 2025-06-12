#ifndef BACKEND_INTERFACE_INCLUDED
#define BACKEND_INTERFACE_INCLUDED

#include "taskman/backend/IProcessItemModel.h"
#include "taskman/backend/IConnectionTab.h"
#include <QList>
#include <QSharedPointer>

class IBackend {
public:
    virtual IProcessItemModel* createProcessItemModel() = 0;
    virtual QSharedPointer<IPlatform> getPlatform() = 0;
    virtual IConnectionTab* createConnectionTab() = 0;
};

#endif /* BACKEND_INTERFACE_INCLUDED */
