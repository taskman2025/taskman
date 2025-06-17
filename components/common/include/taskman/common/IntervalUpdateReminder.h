#ifndef IntervalUpdateReminder_INCLUDED
#define IntervalUpdateReminder_INCLUDED

#include "taskman/common/ThreadsafeConstReadProxy.h"
#include <QDateTime>
#include <QMutex>
#include <QObject>
#include <QTimer>

/**
 * This class is thread-safe.
 */
class IntervalUpdateReminder : public QObject {
    Q_OBJECT

public:
    IntervalUpdateReminder(qint64 updateIntervalInSeconds);
    ~IntervalUpdateReminder();
    bool needUpdateNow() const;

public slots:
    void start();
    void pause();
    void setInterval(qint64 updateIntervalInSeconds);

signals:
    /**
     * lastUpdateWasAWhileAgo will be true
     * if the current update does not happen
     * after a regular interval since the
     * last update. This is particularly
     * helpful for process tree diff'ing
     * tool: if lastUpdateWasAWhileAgo is true
     * then they should not diff the new
     * tree against the old one, since
     * the old one is obsolete and no longer
     * relevant.
     */
    void mustUpdateNow(bool lastUpdateWasAWhileAgo);

private:
    mutable QMutex m_mutex;
    qint64 m_updateIntervalInSeconds;
    QDateTime m_lastUpdateTime;
    QTimer m_timer;
    bool m_neverUpdated;

private slots:
    void issueUpdate();
};

#endif // IntervalUpdateReminder_INCLUDED
