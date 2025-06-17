#ifndef ProcessTreeBuilder_INCLUDED
#define ProcessTreeBuilder_INCLUDED

#include "taskman/platform_runtimes/IPlatformRuntime.h"
#include <QObject>
#include <QMutex>
#include "taskman/common/ThreadsafeSharedConstPointer.h"
#include "taskman/backends/ProcessTree.h"

class ProcessTreeBuilder : public QObject {
    Q_OBJECT

public:
    ProcessTreeBuilder(IPlatformRuntime& runtime);
    ~ProcessTreeBuilder();
    bool isBuilding();

public slots:
    /**
     * Will only initiate build if there is no build in progress
     */
    void build();

signals:
    void buildFinished(ThreadsafeSharedConstPointer<ProcessTree> tree);

private:
    IPlatformRuntime& m_runtime;
    QMutex m_buildMutex;
};

#endif // ProcessTreeBuilder_INCLUDED
