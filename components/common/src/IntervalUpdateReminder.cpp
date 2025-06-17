#include "taskman/common/IntervalUpdateReminder.h"
#include <QMutexLocker>
#include <QtConcurrent>

IntervalUpdateReminder::IntervalUpdateReminder(qint64 updateIntervalInSeconds)
    : m_updateIntervalInSeconds{updateIntervalInSeconds},
      m_lastUpdateTime{},
      m_timer{},
      m_neverUpdated{true} {
    m_timer.setInterval(updateIntervalInSeconds * 1000);
    connect(&m_timer, &QTimer::timeout, this, &IntervalUpdateReminder::issueUpdate);
}

#define GUARD QMutexLocker guard{&m_mutex};

IntervalUpdateReminder::~IntervalUpdateReminder() {
    // GUARD
    pause();
}

void IntervalUpdateReminder::start() {
    GUARD
    m_timer.start();
}

void IntervalUpdateReminder::pause() {
    GUARD
    m_timer.stop();
}

bool IntervalUpdateReminder::needUpdateNow() const {
    GUARD
    return m_lastUpdateTime.secsTo(QDateTime::currentDateTime()) >= m_updateIntervalInSeconds;
}

void IntervalUpdateReminder::issueUpdate() {
    GUARD
    QDateTime previousUpdateTime = QDateTime::currentDateTime();
    std::swap(m_lastUpdateTime, previousUpdateTime);
    QDateTime& currentUpdateTime = m_lastUpdateTime;

    bool lastUpdateWasAWhileAgo = m_neverUpdated || (
        previousUpdateTime.secsTo(currentUpdateTime) >= m_updateIntervalInSeconds + 2
    );
    m_neverUpdated = false;

    emit mustUpdateNow(lastUpdateWasAWhileAgo);
}

void IntervalUpdateReminder::setInterval(qint64 updateIntervalInSeconds) {
    GUARD
    if (updateIntervalInSeconds != m_updateIntervalInSeconds) {
        m_updateIntervalInSeconds = updateIntervalInSeconds;
        m_timer.setInterval(updateIntervalInSeconds * 1000);
    }
}

#undef GUARD
