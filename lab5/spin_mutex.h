#pragma once
// Самодельный мьютекс на атомарной переменной и compare_exchange.
#include <atomic>
#include <thread>

class SpinMutex {
public:
    void lock()
    {
        bool expected = false;
        // false -> true получилось атомарно — мьютекс наш.
        // Не получилось — в expected записано true, сбрасываем и пробуем снова.
        while (!locked.compare_exchange_weak(expected, true)) {
            expected = false;
            std::this_thread::yield();
        }
    }

    void unlock()
    {
        locked.store(false);
    }

private:
    std::atomic<bool> locked = false;
};
