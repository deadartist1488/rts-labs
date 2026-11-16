#pragma once
// Самодельный мьютекс на атомарной переменной и compare_exchange.
#include <atomic>
#include <thread>

class SpinMutex {
public:
    SpinMutex() = default;
    SpinMutex(const SpinMutex&) = delete;
    SpinMutex& operator=(const SpinMutex&) = delete;

    void lock() {
        bool expected = false;
        // Пытаемся атомарно поменять false -> true. Получилось — мьютекс наш.
        // При неудаче compare_exchange_weak записывает в expected текущее
        // значение (true), поэтому сбрасываем его обратно в false.
        while (!locked_.compare_exchange_weak(expected, true,
                                              std::memory_order_acquire,
                                              std::memory_order_relaxed)) {
            expected = false;
            // Пока занято — только читаем (не гоняем кэш-линию записью)
            // и отдаём квант другим потокам.
            while (locked_.load(std::memory_order_relaxed))
                std::this_thread::yield();
        }
    }

    bool try_lock() {
        bool expected = false;
        return locked_.compare_exchange_strong(expected, true,
                                               std::memory_order_acquire,
                                               std::memory_order_relaxed);
    }

    void unlock() {
        locked_.store(false, std::memory_order_release);
    }

private:
    std::atomic<bool> locked_{ false };
};
