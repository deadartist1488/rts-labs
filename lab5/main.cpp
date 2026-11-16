// Лабораторная №5: решение лаб.4 с самодельным мьютексом вместо std::mutex.
#include <cstdlib>
#include <thread>
#include <vector>
#include "common.h"
#include "spin_mutex.h"
#include "fatties.h"     // из lab4, параметризован типом мьютекса

// Отдельная проверка мьютекса: 4 потока по 1 000 000 инкрементов общего счётчика.
template <class Mutex>
long long counter_test(bool use_lock)
{
    Mutex m;
    volatile long long counter = 0;
    std::vector<std::thread> th;
    for (int t = 0; t < 4; ++t)
        th.emplace_back([&] {
            for (int i = 0; i < 1'000'000; ++i) {
                if (use_lock) m.lock();
                counter = counter + 1;
                if (use_lock) m.unlock();
            }
        });
    for (auto& x : th) x.join();
    return counter;
}

int main(int argc, char** argv)
{
    setup_console();
    std::cout << "Лаб.5 «Самодельный мьютекс» (" << BUILD_CONFIG << ")\n\n";

    std::cout << "Проверка: 4 потока x 1 000 000 инкрементов (ожидается 4000000)\n"
              << "  без блокировки: " << counter_test<SpinMutex>(false) << "\n"
              << "  с SpinMutex:    " << counter_test<SpinMutex>(true) << "\n\n";

    int repeats = argc > 1 ? std::atoi(argv[1]) : 1;
    int bad = run_all<SpinMutex>("SpinMutex (atomic<bool> + compare_exchange_weak)", repeats);
    std::cout << (bad == 0 ? "Все сценарии дали ожидаемый исход.\n"
                           : "ЕСТЬ НЕОЖИДАННЫЕ ИСХОДЫ!\n");
    system("pause");
    return bad;
}
