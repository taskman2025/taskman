#ifndef ThreadsafeConstReadProxy_INCLUDED
#define ThreadsafeConstReadProxy_INCLUDED

#include "taskman/common/ThreadsafeSharedConstPointer.h"
#include <QMutex>
#include <QMutexLocker>

/**
 * This class template provides a way to read
 * and write a variable with guaranteed
 * thread-safety.
 *
 * It works as follows: the get() method returns
 * a smart pointer, whose data can be read from
 * until the pointer goes out of scope.
 *
 * The set() method changes the smart pointer to
 * be returned by the very next call to get().
 * The pointers obtained by previous calls to get()
 * will still point to the OLD data, while
 * subsequent calls would point to the NEW data.
 * (This is different from, say, using mutex
 * to guard both reads and writes - which would
 * result in the get() method always returning
 * up-to-date data. In this case, we just guard
 * writing while allowing unlimited reading. So
 * once the pointer is obtained, reading the data
 * that it points to needs no mutex, which is
 * faster than the aforementioned traditional
 * approach, i.e. now we optimize for reading,
 * accepting the tradeoff that the retrieved data
 * might be out-of-date.)
 *
 * Thanks to synchronisation primitives, you
 * won't get race condition when using those
 * get() and set() in different threads.
 */
template <typename T>
class ThreadsafeConstReadProxy final {
public:
    ThreadsafeConstReadProxy();
    void set(T* ptr);
    void set(ThreadsafeSharedConstPointer<T> ptr);
    ThreadsafeSharedConstPointer<T> get() const;

private:
    ThreadsafeSharedConstPointer<T> m_currentPtr;
    mutable QMutex m_mutex;
};

template <typename T>
ThreadsafeConstReadProxy<T>::ThreadsafeConstReadProxy()
    : m_mutex{}, m_currentPtr{} {}

template <typename T>
void ThreadsafeConstReadProxy<T>::set(T* ptr) {
    QMutexLocker guard{&m_mutex};
    m_currentPtr = ThreadsafeSharedConstPointer<T>{ptr};
}

template <typename T>
void ThreadsafeConstReadProxy<T>::set(ThreadsafeSharedConstPointer<T> ptr) {
    QMutexLocker guard{&m_mutex};
    m_currentPtr = ptr;
}

template <typename T>
ThreadsafeSharedConstPointer<T> ThreadsafeConstReadProxy<T>::get() const {
    QMutexLocker guard{&m_mutex};
    return m_currentPtr;
}

#endif // ThreadsafeConstReadProxy_INCLUDED
