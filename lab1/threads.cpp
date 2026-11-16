// Пункт 1.3 «Упражнение 2»: два потока, считающие факториал, против двух
// последовательных вызовов той же функции.
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include "common.h"

using namespace std;

const long long REPEAT = 10'000'000;   // как в п.1.2: 10! 10 млн раз
mutex m;

unsigned long long factorial(int n)
{
    unsigned long long r = 1;
    for (int k = 2; k <= n; ++k)
        r *= k;
    return r;
}

// Функция потока: 10 млн раз считает 10!.
// n объявлен volatile, чтобы в Release компилятор не вычислил 10! на этапе
// компиляции и не выкинул цикл — иначе сравнивать нечего (см. п.1.2).
void Func(string name)
{
    volatile int n = 10;
    unsigned long long result = 0;
    auto t0 = chrono::steady_clock::now();
    for (long long j = 0; j < REPEAT; ++j)
        result = factorial(n);
    chrono::duration<double> d = chrono::steady_clock::now() - t0;

    m.lock();
    cout << "  " << name << ": " << n << "! = " << result
         << ", время потока " << fixed << setprecision(3) << d.count() << " c"
         << " (thread id " << this_thread::get_id() << ")\n";
    m.unlock();
}

int main()
{
    setup_console();
    cout << "Пункт 1.3 (" << BUILD_CONFIG << "), ядер: " << thread::hardware_concurrency() << "\n";

    // --- 1. Параллельно, в двух потоках ---
    cout << "Параллельно (2 потока):\n";
    clock_t c0 = clock();
    auto t0 = chrono::steady_clock::now();
    {
        std::thread thread1(Func, "t1");
        std::thread thread2(Func, "t2");
        thread1.join();
        thread2.join();
    }
    chrono::duration<double> par = chrono::steady_clock::now() - t0;
    double par_clock = double(clock() - c0) / CLOCKS_PER_SEC;

    // --- 2. Последовательно, два вызова в главном потоке ---
    cout << "Последовательно (2 вызова):\n";
    c0 = clock();
    t0 = chrono::steady_clock::now();
    Func("call1");
    Func("call2");
    chrono::duration<double> seq = chrono::steady_clock::now() - t0;
    double seq_clock = double(clock() - c0) / CLOCKS_PER_SEC;

    cout << fixed << setprecision(3)
         << "\nИтого:\n"
         << "  параллельно:     steady_clock " << par.count() << " c, clock() " << par_clock << " c\n"
         << "  последовательно: steady_clock " << seq.count() << " c, clock() " << seq_clock << " c\n"
         << "  ускорение: " << setprecision(2) << seq.count() / par.count() << "x\n";

    system("pause");
    return 0;
}
