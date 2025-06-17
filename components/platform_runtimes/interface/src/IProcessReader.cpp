#include "taskman/platform_runtimes/IProcessReader.h"
#include <cassert>

IProcessReader::IProcessReader() : m_finished{true}, m_firstTime{true} {}

IProcessReader::~IProcessReader() = default;

#define ASSERT_NOT_FINISHED assert(!m_finished);

bool IProcessReader::next() {
    if (m_firstTime) {
        m_firstTime = false;
    } else {
        ASSERT_NOT_FINISHED
    }
    m_finished = !doNext();
    return !m_finished;
}

bool IProcessReader::isFinished() const {
    return m_finished;
}

proc_id_t IProcessReader::getCurrentPID() const {
    ASSERT_NOT_FINISHED
    return doGetCurrentPID();
}

proc_id_t IProcessReader::getCurrentPPID() const {
    ASSERT_NOT_FINISHED
    return doGetCurrentPPID();
}

QVariant IProcessReader::getCurrentProcData(field_mask_t fieldBit) const {
    ASSERT_NOT_FINISHED
    return doGetCurrentProcData(fieldBit);
}
