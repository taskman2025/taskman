#ifndef ProcessAction_INCLUDED
#define ProcessAction_INCLUDED

#include "taskman/common/types.h"

#include <QString>
#include <QList>

struct ProcessAction {
    process_action_id_t id;
    QString name;
    QString description;
    QString confirmationMessage;
    QList<field_mask_t> confirmationMessageArguments;
    // TODO: arguments, e.g. for action "set nice" on Linux, one argument (new nice value) is required.
};

#endif // ProcessAction_INCLUDED
