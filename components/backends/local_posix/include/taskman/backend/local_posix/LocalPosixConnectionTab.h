#ifndef LocalPosixConnectionTab_INCLUDED
#define LocalPosixConnectionTab_INCLUDED

#include "taskman/backend/IConnectionTab.h"
#include "taskman/common_ui/FileInputWidget.h"

class LocalPosixConnectionTab : public IConnectionTab {
public:
    LocalPosixConnectionTab(IBackend* backend);

private:
    FileInputWidget* fileInput;
};

#endif // LocalPosixConnectionTab_INCLUDED
