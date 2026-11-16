// Лабораторная №2: три потока в течение 1 секунды прибавляют 1e-9 к числу.
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>
#include <cstdlib>
#include "common.h"

using namespace std;

#ifndef NO_MUTEX
std::mutex m;
#endif

void Func(string name)
{
    long double i = 0;
    const auto start = chrono::steady_clock::now();
    const auto deadline = start + chrono::seconds(1);

    // Пока не прошла секунда — прибавляем 10^-9.
    // Время проверяется на каждой итерации, поэтому сама проверка тоже
    // «съедает» часть секунды.
    while (chrono::steady_clock::now() < deadline)
        i += 1e-9L;

#ifndef NO_MUTEX
    m.lock();
    std::cout << name << ": " << i << std::endl;
    m.unlock();
#else
    std::cout << name << ": " << i << std::endl;
#endif
}

int main()
{
    setup_console();
#ifdef NO_MUTEX
    cout << "Лаб.2 (" << BUILD_CONFIG << ", БЕЗ мьютекса)" << endl;
#else
    cout << "Лаб.2 (" << BUILD_CONFIG << ", с мьютексом)" << endl;
#endif

    std::thread thread1(Func, "t1");
    std::thread thread2(Func, "t2");
    std::thread thread3(Func, "t3");
    thread1.join();
    thread2.join();
    thread3.join();

    system("pause");
}
