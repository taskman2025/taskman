#ifndef ThreadsafeSharedConstPointer_INCLUDED
#define ThreadsafeSharedConstPointer_INCLUDED

#include <atomic>
#include <mutex>
#include <utility>

template<typename T>
class ThreadsafeSharedConstPointer {
public:
    // Default constructor (null pointer)
    ThreadsafeSharedConstPointer() noexcept : controlBlock(nullptr) {}

    // Constructor from raw pointer (takes ownership)
    explicit ThreadsafeSharedConstPointer(T const* ptr) {
        if (ptr) {
            controlBlock = new ControlBlock(ptr);
        } else {
            controlBlock = nullptr;
        }
    }

    // Copy constructor (increments refcount)
    ThreadsafeSharedConstPointer(const ThreadsafeSharedConstPointer& other) noexcept {
        acquire(other.controlBlock);
    }

    // Move constructor
    ThreadsafeSharedConstPointer(ThreadsafeSharedConstPointer&& other) noexcept {
        controlBlock = other.controlBlock;
        other.controlBlock = nullptr;
    }

    // Copy assignment
    ThreadsafeSharedConstPointer& operator=(const ThreadsafeSharedConstPointer& other) noexcept {
        if (this != &other) {
            release();
            acquire(other.controlBlock);
        }
        return *this;
    }

    // Move assignment
    ThreadsafeSharedConstPointer& operator=(ThreadsafeSharedConstPointer&& other) noexcept {
        if (this != &other) {
            release();
            controlBlock = other.controlBlock;
            other.controlBlock = nullptr;
        }
        return *this;
    }

    // Destructor
    ~ThreadsafeSharedConstPointer() {
        release();
    }

    // Access the raw pointer (read-only)
    T const* get() const noexcept {
        return controlBlock ? controlBlock->ptr : nullptr;
    }

    T const& operator*() const noexcept { return *get(); }
    T const* operator->() const noexcept { return get(); }

    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

private:
    struct ControlBlock {
        T const* ptr;
        std::atomic<size_t> refCount;

        ControlBlock(T const* p) : ptr(p), refCount(1) {}

        ~ControlBlock() {
            delete ptr;
        }
    };

    ControlBlock* controlBlock;

    void acquire(ControlBlock* cb) noexcept {
        controlBlock = cb;
        if (controlBlock) {
            controlBlock->refCount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void release() noexcept {
        if (controlBlock && controlBlock->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete controlBlock;
        }
        controlBlock = nullptr;
    }
};

#endif // ThreadsafeSharedConstPointer_INCLUDED
