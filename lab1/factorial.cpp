// Пункт 1.2 «Упражнение 1»: 10! — 10 млн раз, замер времени в Debug и Release.
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include "common.h"

unsigned long long factorial(int n)
{
    unsigned long long r = 1;
    for (int k = 2; k <= n; ++k)
        r *= k;
    return r;
}

int main()
{
    setup_console();
    const int N = 10;
    const long long REPEAT = 10'000'000;

    std::cout << "Пункт 1.2: " << N << "! x " << REPEAT << " раз (" << BUILD_CONFIG << ")\n";

    clock_t start = clock();
    auto t0 = std::chrono::steady_clock::now();

    unsigned long long result = 0;
    for (long long j = 0; j < REPEAT; ++j)
        result = factorial(N);

    auto t1 = std::chrono::steady_clock::now();
    clock_t end = clock();

    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::chrono::duration<double> wall = t1 - t0;

    std::cout << N << "! = " << result << "\n";
    std::cout << std::fixed << std::setprecision(6)
              << "clock():        " << seconds << " c\n"
              << "steady_clock:   " << wall.count() << " c\n";

    system("pause");
    return 0;
}
