#ifndef ThreadsafeQueue_INCLUDED
#define ThreadsafeQueue_INCLUDED

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class ThreadsafeQueue {
public:
    void push(T const& value) {
        std::unique_lock lock{m_mutex};
        m_queue.push(value);
        m_cv.notify_one();
    }

    std::optional<T> waitAndPop() {
        std::unique_lock lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_queue.empty() || m_done; });
        if (m_queue.empty())
            return std::nullopt;
        T value = m_queue.front();
        m_queue.pop();
        return value;
    }

    /**
     * When this function is called, every consumer thread
     * calling waitAndPop() would get std::nullopt as the
     * return value, thus gracefully exit accordingly.
     */
    void shutDown() {
        std::unique_lock lock(m_mutex);
        m_done = true;
        m_cv.notify_all();
    }

    bool empty() {
        std::unique_lock lock(m_mutex);
        return m_queue.empty();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_done = false;
};

#endif // ThreadsafeQueue_INCLUDED
